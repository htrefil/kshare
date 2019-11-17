#ifndef COMMON_NET_PACKET_HPP
#define COMMON_NET_PACKET_HPP
#include <enet/enet.h>

namespace common {
namespace net {

class packet {
public:
	packet(const packet&) = delete;

	const packet& operator=(const packet&) = delete;

	packet(ENetPacket* packet);

	packet(packet&& other);

	~packet();

	ENetPacket* leak();

	const unsigned char* data() const;

	size_t size() const;

private:
	ENetPacket* packet_;
};

}
}

#endif