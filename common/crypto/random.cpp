#include <system_error>
#include <cerrno>
#include <sys/random.h>

#include "random.hpp"

namespace common {
namespace crypto {

void random(unsigned char* buffer, size_t size) {
	if (getrandom(buffer, size, 0) != (ssize_t)size)
		throw std::system_error(std::error_code(errno, std::generic_category()), "Failed to generate random bytes");
}

}
}