#include <type_traits>

#include "msg.hpp"

namespace common {
namespace proto {

client_msg read_traits<client_msg>::read(reader& reader) {
	using type = std::underlying_type_t<client_msg>;

	auto msg = reader.read<type>();
	if (msg < (type)client_msg::AUTH_RESPONSE || msg > (type)client_msg::LEAVE)
		throw read_error("Invalid client message");

	return (client_msg)msg;
}

void write_traits<client_msg>::write(writer& writer, client_msg msg) {
	writer.write((std::underlying_type_t<client_msg>)msg);
}

server_msg read_traits<server_msg>::read(reader& reader) {
	using type = std::underlying_type_t<server_msg>;

	auto msg = reader.read<type>();
	if (msg < (type)server_msg::AUTH_CHALLENGE || msg > (type)server_msg::MOUSE)
		throw read_error("Invalid server message");

	return (server_msg)msg;
}

void write_traits<server_msg>::write(writer& writer, server_msg msg) {
	writer.write((std::underlying_type_t<server_msg>)msg);
}

mouse_action read_traits<mouse_action>::read(reader& reader) {
	using type = std::underlying_type_t<server_msg>;

	auto action = reader.read<type>();
	if (action < (type)mouse_action::X || action > (type)mouse_action::WHEEL)
		throw read_error("Invalid mouse action");

	return (mouse_action)action;
}

void write_traits<mouse_action>::write(writer& writer, mouse_action action) {
	writer.write((std::underlying_type_t<mouse_action>)action);
}

}
}