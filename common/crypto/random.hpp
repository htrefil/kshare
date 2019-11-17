#ifndef COMMON_CRYPTO_RANDOM_HPP
#define COMMON_CRYPTO_RANDOM_HPP
#include <cstdlib>
#include <array>

namespace common {
namespace crypto {

void random(unsigned char* buffer, size_t size);

template<size_t N>
std::array<unsigned char, N> random() {
	std::array<unsigned char, N> buffer;
	random(buffer.data(), buffer.size());

	return buffer;
}

}
}

#endif