#ifndef COMMON_PROTO_MSG_HPP
#define COMMON_PROTO_MSG_HPP
#include "reader.hpp"
#include "writer.hpp"

namespace common {
namespace proto {

// C2S messages.
enum class client_msg : unsigned char {
	AUTH_RESPONSE = 0,
	REGISTER,
	LEAVE,
};

template<>
struct read_traits<client_msg> {
	static client_msg read(reader& reader);
};

template<>
struct write_traits<client_msg> {
	static void write(writer& writer, client_msg msg);
};

// S2C messages.
enum class server_msg : unsigned char {
	AUTH_CHALLENGE = 0,
	INIT,
	ENTER,
	LEAVE,
	SYNC,
	KEY,
	MOUSE,
};

template<>
struct read_traits<server_msg> {
	static server_msg read(reader& reader);
};

template<>
struct write_traits<server_msg> {
	static void write(writer& writer, server_msg msg);
};

enum class mouse_action : unsigned char {
	X = 0,
	Y,
	WHEEL,
};

template<>
struct read_traits<mouse_action> {
	static mouse_action read(reader& reader);
};

template<>
struct write_traits<mouse_action> {
	static void write(writer& writer, mouse_action action);
};

}
}

#endif