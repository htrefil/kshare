#ifndef COMMON_PROTO_WRITER_HPP
#define COMMON_PROTO_WRITER_HPP
#include <array>

#include "../crypto/key.hpp"
#include "../crypto/iv.hpp"
#include "../net/packet.hpp"

namespace common {
namespace proto {

template<typename T>
struct write_traits;

class writer {
public:
	writer(crypto::key key);

	~writer();

	writer(const writer&) = delete;

	writer& operator=(const writer&) = delete;

	void write_data(const unsigned char* data, size_t size);

	template<typename T>
	void write(T e) {
		write_traits<T>::write(*this, e);
	}

	net::packet finish();

private:
	static constexpr size_t GROW = 8;

	crypto::key key_;
	crypto::iv iv_;
	unsigned char* data_;
	size_t size_;
	size_t capacity_;
};

template<typename T>
struct write_traits {};

template<>
struct write_traits<const char*> {
	static void write(writer& writer, const char* str);
};

template<>
struct write_traits<std::pair<const unsigned char*, size_t>> {
	static void write(writer& writer, std::pair<const unsigned char*, size_t> data);
};

template<>
struct write_traits<bool> {
	static void write(writer& writer, bool data);
};

template<>
struct write_traits<unsigned char> {
	static void write(writer& writer, unsigned char data);
};

template<>
struct write_traits<int16_t> {
	static void write(writer& writer, int16_t data);
};

template<>
struct write_traits<uint16_t> {
	static void write(writer& writer, uint16_t data);
};

template<>
struct write_traits<int32_t> {
	static void write(writer& writer, int32_t data);
};

template<>
struct write_traits<uint32_t> {
	static void write(writer& writer, uint32_t data);
};

template<typename T, typename U>
struct write_traits<std::pair<T, U>> {
	static void write(writer& writer, const std::pair<T, U>& p) {
		const auto& [a, b] = p;

		writer.write<T>(a);
		writer.write<U>(b);
	}
};

template<typename T, size_t N>
struct write_traits<std::array<T, N>> {
	static void write(writer& writer, const std::array<T, N>& data) {
		for (const auto& e : data)
			writer.write<T>(e);
	}
};

template<typename T>
void write_args(writer& writer, const T& arg) {
	writer.write<T>(arg);
}

template<typename T, typename... Args>
void write_args(writer& writer, T first, const Args&... args) {
	writer.write<T>(first);
	write_args(writer, args...);
}

template<typename... Args>
net::packet make_packet(crypto::key key, const Args&... args) {
	writer writer(key);
	write_args(writer, args...);

	return writer.finish();
}

}
}

#endif