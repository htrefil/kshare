#ifndef SERVER_HPP
#define SERVER_HPP
#include <common/net/host.hpp>
#include <common/proto/writer.hpp>
#include <common/crypto/key.hpp>
#include <common/input/event_manager.hpp>
#include <common/x11/context.hpp>

#include "config.hpp"
#include "client_state.hpp"

class server {
public:
	server(uint16_t port, const char* password, const client_set& clients);

	void process();

private:
	bool input_callback(const input_event& event);

	void handle_connect(common::net::peer<client_state>& peer);

	void handle_disconnect(common::net::peer<client_state>& peer);

	void handle_receive(common::net::peer<client_state>& peer, const common::net::packet& data);

	template<typename... Args> 
	void send(common::net::peer<client_state>& peer, const Args&... args) {
		const auto& key = key_for(peer);

		peer.send(common::proto::make_packet(key, args...));
	}

	const common::crypto::key& key_for(common::net::peer<client_state>& peer);

	static constexpr int TIMEOUT = 1;

	common::net::host<client_state> host_;
	common::crypto::key key_;
	const client_set& clients_;
	common::input::event_manager event_manager_;
	common::x11::context context_;
	common::net::peer<client_state>* current_;
	bool ignore_mouse_;
};

#endif
