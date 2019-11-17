#ifndef COMMON_CRYPTO_CONTEXT_HPP
#define COMMON_CRYPTO_CONTEXT_HPP
#include <aes.hpp>

#include "iv.hpp"
#include "key.hpp"

namespace common {
namespace crypto {

static constexpr size_t BLOCK_SIZE = 16;

class context {
public:
	context(const key& key, const iv& iv);

	void crypt(unsigned char* data, size_t size);

private:
	AES_ctx ctx_;
};

}
}

#endif