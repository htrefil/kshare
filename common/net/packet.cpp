#include "packet.hpp"

namespace common {
namespace net {

packet::packet(ENetPacket* packet) : packet_(packet) {}

packet::packet(packet&& other) : packet_(other.packet_) {
	other.packet_ = nullptr;
}

packet::~packet() {
	if (packet_ != nullptr)
		enet_packet_destroy(packet_);
}

ENetPacket* packet::leak() {
	auto p = packet_;
	packet_ = nullptr; 

	return p;
}

const unsigned char* packet::data() const {
	return packet_->data;
}

size_t packet::size() const {
	return packet_->dataLength;
}

}
}