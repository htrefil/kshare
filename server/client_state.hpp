#ifndef CLIENT_STATE_HPP
#define CLIENT_STATE_HPP
#include <variant>
#include <common/crypto/context.hpp>
#include <cstdint>

#include "config.hpp"

class client_state_auth {
public:
	client_state_auth();

	const std::array<unsigned char, common::crypto::BLOCK_SIZE>& challenge() const;

public:
	std::array<unsigned char, common::crypto::BLOCK_SIZE> challenge_;
};

class client_state_registering {};

class client_state_registered {
public:
	client_state_registered(std::string name, uint16_t width, uint16_t height, client_position position);

	const char* name() const;

	uint16_t width() const;

	uint16_t height() const;
	
	client_position position() const;

private:
	std::string name_;
	uint16_t width_;
	uint16_t height_;
	client_position position_;
};

using client_state_variant = std::variant<client_state_auth, client_state_registering, client_state_registered>;

class client_state : public client_state_variant {
public:
	using client_state_variant::operator=;

	client_state();
};

#endif