#ifndef COMMON_INPUT_EVENT_WRITER_HPP
#define COMMON_INPUT_EVENT_WRITER_HPP
#include <linux/input.h>
#include <system_error>

namespace common {
namespace input {

class event_writer {
public:
	event_writer();

	~event_writer();

	void write(const input_event& event);

	void write(__u16 type, __u16 code, __s32 value);

private:
	template<typename... Args>
	static void do_ioctl(int fd, int request, Args... args) {
		if (ioctl(fd, request, args...) == -1)
			throw std::system_error(std::error_code(errno, std::generic_category()), "Failed to do ioctl");
	}

	int fd_;
};

}
}

#endif