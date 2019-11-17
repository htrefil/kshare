#include <toml.hpp>

#include "config.hpp"

config::config(const char* path) {
	auto data = toml::parse(path);

	server_ = toml::find<std::string>(data, "server");
	port_ = toml::find<uint16_t>(data, "port");
	password_ = toml::find<std::string>(data, "password");
	name_ = toml::find<std::string>(data, "name");
}

const char* config::server() const {
	return server_.c_str();
}

uint16_t config::port() const {
	return port_;
}

const char* config::password() const {
	return password_.c_str();
}

const char* config::name() const {
	return name_.c_str();
}