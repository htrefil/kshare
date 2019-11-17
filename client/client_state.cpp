#include "client_state.hpp"

client_state_init::client_state_init(std::pair<uint16_t, uint16_t> x_range, std::pair<uint16_t, uint16_t> y_range) : x_range_(x_range), y_range_(y_range), entered_(false) {}

const std::pair<uint16_t, uint16_t>& client_state_init::x_range() const {
	return x_range_;
}

const std::pair<uint16_t, uint16_t>& client_state_init::y_range() const {
	return y_range_;
}

bool client_state_init::entered() const {
	return entered_;
}

void client_state_init::set_entered(bool value) {
	entered_ = value;
}

client_state::client_state() : client_state_variant(client_state_auth()) {}