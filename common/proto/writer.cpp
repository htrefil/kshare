#include <cstring>
#include <stdexcept>
#include <array>

#include "../crypto/context.hpp"
#include "../crypto/random.hpp"
#include "writer.hpp"

template<typename T>
static void write_integer(common::proto::writer& writer, T data) {
	unsigned char buffer[sizeof(data)];
	for (size_t i = 0; i < sizeof(data); i++)
		buffer[i] = (data >> ((sizeof(data) - i - 1) * 8)) & (T)0xFF;

	writer.write_data(buffer, sizeof(buffer));
}

namespace common {
namespace proto {

writer::writer(crypto::key key) : key_(key), iv_(crypto::random<crypto::IV_SIZE>()), data_(nullptr), size_(0), capacity_(0) {
	write(iv_);
}

writer::~writer() {
	delete[] data_;
}

void writer::write_data(const unsigned char* data, size_t size) {
	if (size > capacity_) {
		auto new_data = new unsigned char[size_ + size + GROW];

		memcpy(new_data, data_, size_);
		memcpy(new_data + size_, data, size);

		delete[] data_;
		data_ = new_data;

		size_ += size;
		capacity_ = GROW;
	} else {
		memcpy(data_ + size_, data, size);		

		size_ += size;
		capacity_ -= size;
	}
}

net::packet writer::finish() {
	static constexpr unsigned char PADDING[crypto::BLOCK_SIZE] = { 0 };
	write_data(PADDING, crypto::BLOCK_SIZE - (size_ - iv_.size()) % crypto::BLOCK_SIZE);

	crypto::context context(key_, iv_);
	context.crypt(data_ + iv_.size(), size_ - iv_.size());

	auto packet = enet_packet_create(data_, size_, ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);
	if (packet == nullptr)
		throw std::runtime_error("Failed to create packet");

	packet->freeCallback = [](ENetPacket* packet) {
		delete[] (unsigned char*)packet->data;
	};

	data_ = nullptr;
	size_ = 0;
	capacity_ = 0;

	return net::packet(packet);
}

void write_traits<const char*>::write(writer& writer, const char* str) {
	writer.write_data((unsigned char*)str, strlen(str) + 1);
}

void write_traits<std::pair<const unsigned char*, size_t>>::write(writer& writer, std::pair<const unsigned char*, size_t> data) {
	writer.write_data(data.first, data.second);
}

void write_traits<bool>::write(writer& writer, bool data) {
	writer.write<unsigned char>(data ? 1 : 0);
}

void write_traits<unsigned char>::write(writer& writer, unsigned char data) {
	write_integer(writer, data);
}

void write_traits<int16_t>::write(writer& writer, int16_t data) {
	write_integer(writer, data);
}

void write_traits<uint16_t>::write(writer& writer, uint16_t data) {
	write_integer(writer, data);
}

void write_traits<int32_t>::write(writer& writer, int32_t data) {
	write_integer(writer, data);
}

void write_traits<uint32_t>::write(writer& writer, uint32_t data) {
	write_integer(writer, data);
}

}
}
