#include "event_manager.hpp"

namespace common {
namespace input {

event_manager::event_manager(callback_func callback, int timeout) : callback_(callback), reader_(std::bind(&event_manager::reader_callback, this, std::placeholders::_1), timeout) {}

void event_manager::process() {
	reader_.process();
}

void event_manager::reader_callback(const input_event& event) {
	if (callback_(event))
		writer_.write(event);
}

}
}
