#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>
#include <cstdint>
#include <common/proto/writer.hpp>
#include <common/net/host.hpp>
#include <common/crypto/key.hpp>
#include <common/input/event_writer.hpp>
#include <common/x11/context.hpp>

#include "client_state.hpp"

class client {
public:
	client(const char* server, uint16_t port, const char* password, const char* name);

	void process();

private:
	void handle_connect();

	void handle_disconnect();

	void handle_receive(const common::net::packet& data);

	const common::crypto::key& current_key() const;

	template<typename... Args>
	void send(const Args&... args) {
		peer_.send(common::proto::make_packet(current_key(), args...));
	}

	static constexpr uint32_t TIMEOUT = 1000;

	common::net::host<client_state> host_;
	common::net::peer<client_state>& peer_;
	common::crypto::key key_;
	const char* name_;
	common::input::event_writer writer_;
	common::x11::context context_;
};

#endif