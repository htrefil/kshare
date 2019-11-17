#ifndef COMMON_NET_PEER_HPP
#define COMMON_NET_PEER_HPP
#include <enet/enet.h>
#include <stdexcept>

#include "packet.hpp"

namespace common {
namespace net {

template<typename T>
class host;

template<typename T>
class peer {
public:
	void disconnect() {
		enet_peer_disconnect(peer_, 0);
	}

	T& data() {
		return data_;
	}

	const T& data() const {
		return data_;
	}

	void send(packet packet) {
		if (enet_peer_send(peer_, ENET_PACKET_FLAG_RELIABLE, packet.leak()) < 0)
			throw std::runtime_error("Failed to send packet");
	}

	const char* address() const {
		return address_;
	}

private:
	template<typename> 
	friend class host;

	peer(ENetPeer* peer) : peer_(peer) {
		if (enet_address_get_host_ip(&peer->address, address_, sizeof(address_)) < 0)
			throw std::runtime_error("Failed to get peer IP address");
	}

	ENetPeer* peer_;
	char address_[15 + 1];
	T data_;
};

}
}

#endif