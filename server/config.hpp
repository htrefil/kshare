#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <toml.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <cstdint>

enum class client_position {
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
};

class client_set {
public:
	std::optional<client_position> get(const char* name) const;

	size_t length() const;

private:
	friend class toml::from<client_set>; 
	friend class config;
	
	client_set() = default;

	std::unordered_map<std::string, client_position> positions_;
};

class config {
public:
	config(const char* path);

	uint16_t port() const;

	const char* password() const;

	const client_set& clients() const;

private:
	uint16_t port_;
	std::string password_;
	client_set clients_;
};

#endif