#include "reader.hpp"

#include <cstring>
#include <algorithm>

#include "reader.hpp"
#include "../crypto/iv.hpp"

template<typename T>
static T read_integer(common::proto::reader& reader) {
	unsigned char buffer[sizeof(T)];
	reader.read_data(buffer, sizeof(buffer));

	T n = 0;
	for (size_t i = 0; i < sizeof(T); i++)
		n |= (T)buffer[i] << ((sizeof(T) - i - 1) * 8);

	return n;
}

namespace common {
namespace proto {

read_error::read_error(const char* what) : std::runtime_error(what) {}

reader::reader(const crypto::key& key, const unsigned char* data, size_t size) : position_(0) {
	crypto::iv iv;
	if (size < iv.size() || ((size - iv.size()) % block_.size()) != 0)
		throw read_error("Invalid packet size");
	memcpy(iv.data(), data, iv.size());

	context_ = crypto::context(key, iv);

	data_ = data + iv.size();
	size_ = size - iv.size();
}

void reader::read_data(unsigned char* buffer, size_t size) {
	for (size_t done = 0; done < size; ) {
		auto remaining = position_ % block_.size();

		if (remaining == 0) {
			if (position_ == size_)
				throw read_error("Reached end of packet");

			memcpy(block_.data(), data_ + position_, block_.size());
			context_->crypt(block_.data(), block_.size());

			remaining = block_.size();
		}

		auto n = std::min(remaining, size - done);
		memcpy(buffer + done, block_.data() + position_ % block_.size(), n);

		done += n;
		position_ += n;
	}
}

std::string read_traits<std::string>::read(reader& reader) {
	std::string str;

	char c;
	while ((c = (char)reader.read<unsigned char>()) != 0) 
		str += c;

	return str;
}

bool read_traits<bool>::read(reader& reader) {
	auto b = reader.read<unsigned char>();

	switch (b) {
		case 0:
			return false;

		case 1:
			return true;

		default:
			throw read_error("Invalid bool value");
	}
}

unsigned char read_traits<unsigned char>::read(reader& reader) {
	return read_integer<unsigned char>(reader);
}

int16_t read_traits<int16_t>::read(reader& reader) {
	return read_integer<int16_t>(reader);
}

uint16_t read_traits<uint16_t>::read(reader& reader) {
	return read_integer<uint16_t>(reader); 
}

int32_t read_traits<int32_t>::read(reader& reader) {
	return read_integer<int32_t>(reader);
}

uint32_t read_traits<uint32_t>::read(reader& reader) {
	return read_integer<uint32_t>(reader);
}

}
}
