#include <stdexcept>
#include <common/logger.hpp>
#include <common/proto/msg.hpp>
#include <common/crypto/context.hpp>
#include <common/crypto/random.hpp>

#include "client.hpp"

client::client(const char* server, uint16_t port, const char* password, const char* name) : host_(1, TIMEOUT), peer_(host_.connect(server, port)), key_(common::crypto::make_key(password)), name_(name) {}

void client::process() {
	auto event = host_.process();
	if (!event)
		return;

	if (auto evt = std::get_if<common::net::event_connect>(&*event); evt != nullptr)
		handle_connect();
	else if (auto evt = std::get_if<common::net::event_disconnect>(&*event); evt != nullptr)
		handle_disconnect();
	else if (auto evt = std::get_if<common::net::event_receive>(&*event); evt != nullptr)
		handle_receive(evt->data());
}

void client::handle_connect() {
	common::logger::get().info() << "Connected to server" << std::endl;
}

void client::handle_disconnect() {
	throw std::runtime_error("Disconnected from server");
}

void client::handle_receive(const common::net::packet& data) {
	try {
		common::proto::reader reader(current_key(), data.data(), data.size());

		switch (reader.read<common::proto::server_msg>()) {
			case common::proto::server_msg::AUTH_CHALLENGE: {
				std::get<client_state_auth>(peer_.data());

				auto challenge = reader.read<std::array<unsigned char, common::crypto::BLOCK_SIZE>>();
				auto iv = common::crypto::random<common::crypto::IV_SIZE>();

				common::crypto::context context(key_, iv);
				context.crypt(challenge.data(), challenge.size());

				send(common::proto::client_msg::AUTH_RESPONSE, challenge, iv);

				peer_.data() = client_state_registered();

				send(common::proto::client_msg::REGISTER, name_, context_.width(), context_.height());

				common::logger().get().debug() << "Registered" << std::endl;
				break;
			}

			case common::proto::server_msg::INIT: {
				std::get<client_state_registered>(peer_.data());

				auto x_range = reader.read<std::pair<uint16_t, uint16_t>>();
				auto y_range = reader.read<std::pair<uint16_t, uint16_t>>();

				peer_.data() = client_state_init(x_range, y_range);

				common::logger().get().debug() << "Initialized" << std::endl;
				break;
			}

			case common::proto::server_msg::ENTER: {
				auto& state = std::get<client_state_init>(peer_.data());
				if (state.entered()) 
					throw std::runtime_error("Server tried to enter the screen but has already entered");

				state.set_entered(true);

				common::logger::get().info() << "Server entered the screen" << std::endl;
				break;
			}

			case common::proto::server_msg::LEAVE: {
				auto& state = std::get<client_state_init>(peer_.data());
				if (!state.entered()) 
					throw std::runtime_error("Server tried to leave the screen but has already left");

				state.set_entered(false);

				common::logger::get().info() << "Server left the screen" << std::endl;
				break;
			}

			case common::proto::server_msg::SYNC: {
				const auto& state = std::get<client_state_init>(peer_.data());
				if (!state.entered())
					throw std::runtime_error("Server sent an unexpected message");

				input_event event = { 0 };
				event.type = EV_SYN;
				
				writer_.write(event);
				break;
			}	

			case common::proto::server_msg::KEY: {
				const auto& state = std::get<client_state_init>(peer_.data());
				if (!state.entered())
					throw std::runtime_error("Server sent an unexpected message");

				auto press = reader.read<bool>();
				auto code = reader.read<int32_t>();

				input_event event = { 0 };
				event.type = EV_KEY;
				event.code = code;
				event.value = press;
				
				writer_.write(event);
				break;
			}

			case common::proto::server_msg::MOUSE: {
				auto& state = std::get<client_state_init>(peer_.data());
				if (!state.entered())
					throw std::runtime_error("Server sent an unexpected message");

				auto action = reader.read<common::proto::mouse_action>();
				auto value = reader.read<int32_t>();

				input_event event = { 0 };
				event.type = EV_REL;
				event.value = value;

				bool check = true;
				switch (action) {
					case common::proto::mouse_action::X:
						event.code = REL_X;
						break;

					case common::proto::mouse_action::Y:
						event.code = REL_Y;
						break;

					case common::proto::mouse_action::WHEEL:
						event.code = REL_WHEEL;
						check = false;
						break;
				}

				writer_.write(event);

				if (check) {
					auto [xs, xe] = state.x_range();
					auto [ys, ye] = state.y_range();

					auto [x, y] = context_.mouse_position();
					if (x >= xs && x <= xe && y >= ys && y <= ye) {
						send(common::proto::client_msg::LEAVE);
						state.set_entered(false);

						common::logger::get().info() << "Left the screen" << std::endl;
					}
				}

				break;
			}
		}
	} catch (const common::proto::read_error& e) {
		throw std::runtime_error("Read error: " + std::string(e.what()));
	} catch (const std::bad_variant_access&) {
		throw std::runtime_error("Server sent an unexpected message");
	}
}

const common::crypto::key& client::current_key() const {
	return std::holds_alternative<client_state_auth>(peer_.data()) ? common::crypto::initial_key() : key_;
}
