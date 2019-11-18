#ifndef COMMON_NET_EVENT_HPP
#define COMMON_NET_EVENT_HPP
#include <variant>
#include <utility>
#include <enet/enet.h>

#include "peer.hpp"
#include "packet.hpp"

namespace common {
namespace net {

template<typename T>
class host;

class event_connect {};

class event_disconnect {};

class event_receive {
public:
	const class packet& data() const;

private:
	template<typename>
	friend class host;

	event_receive(packet data);

	packet data_;
};

using event_variant = std::variant<event_connect, event_disconnect, event_receive>;

template<typename T>
class event : public event_variant {
public:
	peer<T>& source() {
		return source_;
	}

private:
	template<typename>
	friend class host;

	event(event_variant variant, peer<T>& source) : event_variant(std::move(variant)), source_(source) {}

	peer<T>& source_;
};

}
}

#endif