#include <utility>

#include "event.hpp"

namespace common {
namespace net {

const packet& event_receive::data() const {
	return data_;
}

event_receive::event_receive(packet data) : data_(std::move(data)) {}

}
}
