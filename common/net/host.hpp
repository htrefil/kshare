#ifndef COMMON_NET_HOST_HPP
#define COMMON_NET_HOST_HPP
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <optional>
#include <cassert>
#include <functional>
#include <enet/enet.h>

#include "peer.hpp"
#include "event.hpp"
#include "peer_iterator.hpp"

namespace common {
namespace net {

template<typename T>
class host {
public:
	host(uint16_t port, size_t max_peers, uint32_t timeout) : timeout_(timeout) {
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = port;

		host_ = enet_host_create(&address, max_peers, 1, 0, 0);
		if (host_ == nullptr)
			throw std::runtime_error("Failed to create host");
	}

	host(size_t max_peers, uint32_t timeout) : timeout_(timeout) {
		host_ = enet_host_create(nullptr, max_peers, 1, 0, 0);
		if (host_ == nullptr)
			throw std::runtime_error("Failed to create host");
	}

	~host() {
		enet_host_destroy(host_);
	}

	peer<T>& connect(const char* server, uint16_t port) {
		ENetAddress address;
		address.port = port;

		if (enet_address_set_host_ip(&address, server) < 0 && enet_address_set_host(&address, server) < 0) 
			throw std::runtime_error("Failed to resolve host");

		auto peer = enet_host_connect(host_, &address, 1, 0);
		if (peer == nullptr)
			throw std::runtime_error("Failed to connect");

		peer->data = new T;
		return *(class peer<T>*)peer->data;
	}

	std::optional<event<T>> process() {
		if (last_event_ && last_event_->type == ENET_EVENT_TYPE_DISCONNECT) 
			delete (peer<T>*)last_event_->peer->data;
		
		ENetEvent cevent;
		if (enet_host_service(host_, &cevent, timeout_) < 0)
			throw std::runtime_error("Failed to check host service");

		last_event_ = cevent;

		switch (cevent.type) {
			case ENET_EVENT_TYPE_CONNECT:
				cevent.peer->data = new peer<T>(cevent.peer);
				return event<T>(event_connect(), *(peer<T>*)cevent.peer->data);

			case ENET_EVENT_TYPE_DISCONNECT:
				return event<T>(event_disconnect(), *(peer<T>*)cevent.peer->data);

			case ENET_EVENT_TYPE_RECEIVE:
				return event<T>(event_receive(cevent.packet), *(peer<T>*)cevent.peer->data);

			case ENET_EVENT_TYPE_NONE:
				break;
		}

		return std::nullopt;
	}

	peer_iterator<T> begin() {
		return peer_iterator<T>(host_->peers, host_->peers + host_->peerCount);
	}

	peer_iterator<T> end() {
		return peer_iterator<T>(host_->peers + host_->peerCount, host_->peers + host_->peerCount);
	}

private:
	uint32_t timeout_;
	ENetHost* host_;
	std::optional<ENetEvent> last_event_;
};

}
}

#endif