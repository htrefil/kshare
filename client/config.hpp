#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <cstdint>
#include <string>

class config {
public:
	config(const char* path);

	const char* server() const;

	uint16_t port() const;

	const char* password() const;

	const char* name() const;

private:
	std::string server_;
	uint16_t port_;
	std::string password_;
	std::string name_;
};

#endif