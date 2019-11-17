#ifndef COMMON_INPUT_EVENT_MANAGER_HPP
#define COMMON_INPUT_EVENT_MANAGER_HPP
#include <functional>

#include "event_reader.hpp"
#include "event_writer.hpp"

namespace common {
namespace input {

class event_manager {
public:
	using callback_func = std::function<bool(const input_event&)>;

	event_manager(callback_func callback, int timeout);

	void process();

private:
	void reader_callback(const input_event& event);

	callback_func callback_;
	event_reader reader_;
	event_writer writer_;
};

}
}

#endif