#include <common/logger.hpp>
#include <common/proto/msg.hpp>

#include "server.hpp"

server::server(uint16_t port, const char* password, const client_set& clients) : host_(port, clients.length(), TIMEOUT), key_(common::crypto::make_key(password)), clients_(clients), event_manager_(std::bind(&server::input_callback, this, std::placeholders::_1), TIMEOUT), current_(nullptr) {
	common::logger::get().info() << "Listening on port " << port << std::endl;
}

void server::process() {
	event_manager_.process();

	auto event = host_.process();
	if (!event)
		return;

	if (auto evt = std::get_if<common::net::event_connect>(&*event); evt != nullptr) 
		handle_connect(event->source());
	else if (auto evt = std::get_if<common::net::event_disconnect>(&*event); evt != nullptr) 
		handle_connect(event->source());
	else if (auto evt = std::get_if<common::net::event_receive>(&*event); evt != nullptr) 
		handle_receive(event->source(), evt->data());
}

bool server::input_callback(const input_event& event) {
	switch (event.type){
		case EV_REL: {
			switch (event.code) {
				case REL_X:
				case REL_Y: {
					auto width = context_.width();
					auto height = context_.height();

					auto [x, y] = context_.mouse_position();
					if (current_ != nullptr) {
						// Determine if we are leaving the clients screen.
						bool leave;
						switch (std::get<client_state_registered>(current_->data()).position()) {
							case client_position::TOP:
								leave = y != 0;
								break;

							case client_position::BOTTOM:
								leave = y != height - 1;
								break;

							case client_position::LEFT:
								leave = x != 0;
								break;

							case client_position::RIGHT:
								leave = x != width - 1;
								break;
						}

						if (leave) {
							send(*current_, common::proto::server_msg::LEAVE);
							current_ = nullptr;

							return true;
						}

						// Not leaving, relay it to the client.
						auto action = event.code == REL_X ? common::proto::mouse_action::X : common::proto::mouse_action::Y;
						send(*current_, common::proto::server_msg::MOUSE, action, (int32_t)event.value);

						return false;
					}

					// Determine if we are entering a clients screen.
					for (auto& p : host_) {
						auto state = std::get_if<client_state_registered>(&p.data());
						if (state == nullptr)
							continue;

						bool got;
						switch (state->position()) {
							case client_position::LEFT:
								got = x == 0;
								break;

							case client_position::RIGHT:
								got = x == width - 1;
								break;

							case client_position::TOP:
								got = y == 0;
								break;

							case client_position::BOTTOM:
								got = y == height - 1;
								break;
						}

						if (got) {
							current_ = &p;
							send(*current_, common::proto::server_msg::ENTER);

							return false;
						}
					}

					return true;
				}

				case REL_WHEEL:
					if (current_ == nullptr)
						return true;

					send(*current_, common::proto::server_msg::MOUSE, common::proto::mouse_action::WHEEL, (int32_t)event.code);
					return false;

				default:
					return true;
			}
		}

		case EV_KEY: {
			if (current_ == nullptr)
				return true;

			bool press;
			switch (event.value) {
				case 0:
					press = false;
					break;

				case 1:
					press = true;
					break;

				default:
					return false;
			}

			send(*current_, common::proto::server_msg::KEY, press, (int32_t)event.code);
			break;
		}

		case EV_SYN:
			if (current_ == nullptr)
				return true;

			send(*current_, common::proto::server_msg::SYNC);
			break;

		default:
			return true;
	}

	return true;
}

void server::handle_connect(common::net::peer<client_state>& peer) {
	common::logger::get().info() << peer.address() << " connected" << std::endl;

	const auto& challenge = std::get<client_state_auth>(peer.data()).challenge();
	send(peer, common::proto::server_msg::AUTH_CHALLENGE, challenge);
}

void server::handle_disconnect(common::net::peer<client_state>& peer) {
	common::logger::get().info() << peer.address() << " disconnected" << std::endl;

	if (current_ == &peer)
		current_ = nullptr;
}

void server::handle_receive(common::net::peer<client_state>& peer, const common::net::packet& data) {
	try {
		common::proto::reader reader(key_for(peer), data.data(), data.size());

		switch (reader.read<common::proto::client_msg>()) {
			case common::proto::client_msg::AUTH_RESPONSE: {
				const auto& schallenge = std::get<client_state_auth>(peer.data()).challenge();

				auto ochallenge = reader.read<std::array<unsigned char, common::crypto::BLOCK_SIZE>>();
				auto iv = reader.read<common::crypto::iv>();

				common::crypto::context context(key_, iv);
				context.crypt(ochallenge.data(), ochallenge.size());

				if (schallenge != ochallenge) {
					common::logger::get().error() << peer.address() << ": attempt to use an invalid password" << std::endl;
					peer.disconnect();
					break;
				}

				peer.data() = client_state_registering();
				break;
			}

			case common::proto::client_msg::REGISTER: {
				std::get<client_state_registering>(peer.data());

				auto name = reader.read<std::string>();
				auto width = reader.read<uint16_t>();
				auto height = reader.read<uint16_t>();

				auto position = clients_.get(name.c_str());
				if (!position) {
					common::logger::get().error() << peer.address() << ": attempt to register as " << name << ", but no such client is configured" << std::endl;
					peer.disconnect();
					break;
				}

				bool ok = true;
				for (const auto& p : host_) {
					if (auto state = std::get_if<client_state_registered>(&p.data()); state != nullptr && state->name() == name) {
						ok = false;
						break;
					}
				}

				if (!ok) {
					common::logger::get().error() << peer.address() << ": attempt to register as  " << name << " twice" << std::endl;
					peer.disconnect();
					break;
				}

				peer.data() = client_state_registered(name, width, height, *position);

				std::pair<uint16_t, uint16_t> x;
				std::pair<uint16_t, uint16_t> y;
				switch (*position) {
					case client_position::TOP:
						x = { 0, width };
						y = { height - 1, height };
						break;

					case client_position::BOTTOM:
						x = { 0, width };
						y = { 0, 1 };
						break;

					case client_position::LEFT:
						x = { width - 1, width };
						y = { 0, height };
						break;

					case client_position::RIGHT:
						x = { 0, 1 };
						y = { 0, height };
						break;
				}

				send(peer, common::proto::server_msg::INIT, x, y);

				common::logger::get().info() << peer.address() << ": registered as " << name << std::endl;
				break;
			}

			case common::proto::client_msg::LEAVE: {
				const auto& state = std::get<client_state_registered>(peer.data());

				if (current_ != &peer) {
					common::logger::get().error() << peer.address() << ": attempt to leave screen, but is not active" << std::endl;
					peer.disconnect();
					break;
				}

				current_ = nullptr;

				// Move our mouse a little so that we don't automatically enter again.
				static constexpr uint16_t OFFSET = 5;

				auto [x, y] = context_.mouse_position();
				switch (state.position()) {
					case client_position::TOP: 
						y += OFFSET;
						break;
					
					case client_position::BOTTOM:
						y -= OFFSET;
						break;

					case client_position::LEFT:
						x += OFFSET;
						break;

					case client_position::RIGHT:
						x -= OFFSET;
						break;
				}

				context_.set_mouse_position({ x, y });
				
				common::logger::get().info() << peer.address() << ": left the screen" << std::endl;
				break;
			}
		}

		return;
	} catch (common::proto::read_error& e) {
		common::logger::get().error() << peer.address() << ": read error: " << e.what() << std::endl;
	} catch (const std::bad_variant_access&) {
		common::logger::get().error() << peer.address() << ": received an unexpected message" << std::endl;
	}

	peer.disconnect();
}

const common::crypto::key& server::key_for(common::net::peer<client_state>& peer) {
	return std::holds_alternative<client_state_auth>(peer.data()) ? common::crypto::initial_key() : key_;
}