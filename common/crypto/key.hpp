#ifndef COMMON_CRYPTO_KEY_HPP
#define COMMON_CRYPTO_KEY_HPP
#include <array>

namespace common {
namespace crypto {

static constexpr size_t KEY_SIZE = 32;

using key = std::array<unsigned char, KEY_SIZE>;

key make_key(const char* password);

const key& initial_key();

}
}

#endif