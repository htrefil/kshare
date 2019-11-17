#include <iostream>
#include <cstdlib>

#include "logger.hpp"

class nop_buffer : public std::streambuf {
public:
	int overflow(int c) {
		return c;
	}
};

static std::ostream& nop_stream() {
	static nop_buffer buffer;
	static std::ostream stream(&buffer);

	return stream;
}

namespace common {

logger::logger() : debug_(getenv("KSHARE_DEBUG") != nullptr) {}

std::ostream& logger::debug() {
	return debug_ ? (std::cout << "[dbg] ") : nop_stream();
}

std::ostream& logger::info() {
	return std::cout << "[inf] ";
}

std::ostream& logger::error() {
	return std::cout << "[err] ";
}

}