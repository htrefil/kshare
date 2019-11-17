#include <common/crypto/random.hpp>

#include "client_state.hpp"

client_state_auth::client_state_auth() : challenge_(common::crypto::random<common::crypto::BLOCK_SIZE>()) {}

const std::array<unsigned char, common::crypto::BLOCK_SIZE>& client_state_auth::challenge() const {
	return challenge_;
}

client_state_registered::client_state_registered(std::string name, uint16_t width, uint16_t height, client_position position) : name_(name), width_(width), height_(height), position_(position) {}

const char* client_state_registered::name() const { 
	return name_.c_str(); 
}

uint16_t client_state_registered::width() const {
	return width_;
}

uint16_t client_state_registered::height() const {
	return height_;
}

client_position client_state_registered::position() const {
	return position_;
}

client_state::client_state() : client_state_variant(client_state_auth()) {}