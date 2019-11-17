#ifndef COMMON_CRYPTO_IV_HPP
#define COMMON_CRYPTO_IV_HPP
#include <array>

namespace common {
namespace crypto {

static constexpr size_t IV_SIZE = 16;

using iv = std::array<unsigned char, 16>;

}
}

#endif