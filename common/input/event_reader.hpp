#ifndef COMMON_INPUT_EVENT_READER_HPP
#define COMMON_INPUT_EVENT_READER_HPP
#include <functional>
#include <linux/input.h>
#include <poll.h>

namespace common {
namespace input {

class event_reader {
public:
	using callback_func = std::function<void(input_event)>;

	event_reader(callback_func callback, int timeout);

	void process();

private:
	void cleanup();

	struct buffer {
		input_event event_;
		size_t size_;
	};

	callback_func callback_;
	int timeout_;
	std::vector<pollfd> fds_;
	std::vector<buffer> buffers_;
};

}
}

#endif