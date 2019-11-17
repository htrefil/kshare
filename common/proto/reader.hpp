#ifndef COMMON_PROTO_READER_HPP
#define COMMON_PROTO_READER_HPP
#include <stdexcept>
#include <array>
#include <optional>

#include "../crypto/context.hpp"
#include "../crypto/key.hpp"

namespace common {
namespace proto {

class read_error : public std::runtime_error {
public:
	read_error(const char* what);
};

template<typename T>
struct read_traits;

class reader {
public:
	reader(const crypto::key& key, const unsigned char* data, size_t size);

	void read_data(unsigned char* buffer, size_t size);

	template<typename T>
	T read() {
		return read_traits<T>::read(*this);
	}

private:
	const unsigned char* data_;
	size_t size_;

	std::array<unsigned char, crypto::BLOCK_SIZE> block_;
	size_t position_;

	std::optional<crypto::context> context_;
};

template<typename T>
struct read_traits {};

template<>
struct read_traits<std::string> {
	static std::string read(reader& reader);
};

template<>
struct read_traits<bool> {
	static bool read(reader& reader);
};

template<>
struct read_traits<unsigned char> {
	static unsigned char read(reader& reader);
};

template<>
struct read_traits<int16_t> {
	static int16_t read(reader& reader);
};

template<>
struct read_traits<uint16_t> {
	static uint16_t read(reader& reader);
};

template<>
struct read_traits<int32_t> {
	static int32_t read(reader& reader);
};

template<>
struct read_traits<uint32_t> {
	static uint32_t read(reader& reader);
};

template<typename T, typename U>
struct read_traits<std::pair<T, U>> {
	static std::pair<T, U> read(reader& reader) {
		return { reader.read<T>(), reader.read<U>() };
	}
};

template<typename T, size_t N>
struct read_traits<std::array<T, N>> {
	static std::array<T, N> read(reader& reader) {
		std::array<T, N> data;
		for (size_t i = 0; i < N; i++)
			data[i] = reader.read<T>();

		return data;
	}
};

}
}

#endif
