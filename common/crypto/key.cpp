#include <sha256/sha256.h>
#include <cstring>

#include "key.hpp"

namespace common {
namespace crypto {

key make_key(const char* password) {
	key k;

	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, (unsigned char*)password, strlen(password));
	sha256_final(&ctx, k.data());	

	return k;
}

const key& initial_key() {
	static const auto KEY = make_key("kshare");

	return KEY;
}

}
}