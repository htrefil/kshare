#include <cassert>

#include "context.hpp"

namespace common {
namespace crypto {

context::context(const key& key, const iv& iv) {
	AES_init_ctx_iv(&ctx_, key.data(), iv.data());
}

void context::crypt(unsigned char* data, size_t size) {
	assert(size % BLOCK_SIZE == 0);

	AES_CTR_xcrypt_buffer(&ctx_, data, size);
}

}
}