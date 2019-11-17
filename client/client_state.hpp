#ifndef CLIENT_STATE_HPP
#define CLIENT_STATE_HPP
#include <variant>
#include <utility>
#include <cstdint>

class client_state_auth {};

class client_state_registered {};

class client_state_init {
public:
	client_state_init(std::pair<uint16_t, uint16_t> x_range, std::pair<uint16_t, uint16_t> y_range);

	const std::pair<uint16_t, uint16_t>& x_range() const;

	const std::pair<uint16_t, uint16_t>& y_range() const;

	bool entered() const;

	void set_entered(bool value);

private:
	std::pair<uint16_t, uint16_t> x_range_;
	std::pair<uint16_t, uint16_t> y_range_;
	bool entered_;
};

using client_state_variant = std::variant<client_state_auth, client_state_registered, client_state_init>;

class client_state : public client_state_variant {
public:
	using client_state_variant::operator=;

	client_state();
};

#endif
