#ifndef COMMON_LOGGER_HPP
#define COMMON_LOGGER_HPP
#include <ostream>

#include "singleton.hpp"

namespace common {

class logger : public singleton<logger> {
public:
	logger();

	std::ostream& debug();

	std::ostream& info();

	std::ostream& error();

private:
	bool debug_;
};

}

#endif