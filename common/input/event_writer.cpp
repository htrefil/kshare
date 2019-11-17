#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <cerrno>
#include <filesystem>
#include <sys/ioctl.h>
#include <cstring>

#include "event_writer.hpp"

namespace common {
namespace input {

event_writer::event_writer() {
	fd_ = open("/dev/uinput", O_WRONLY);
	if (fd_ == -1)
		throw std::system_error(std::error_code(errno, std::generic_category()), "Failed to open /dev/uinput");

	try {
		do_ioctl(fd_, UI_SET_EVBIT, EV_KEY);
		do_ioctl(fd_, UI_SET_EVBIT, EV_MSC);
		do_ioctl(fd_, UI_SET_EVBIT, EV_SYN);
		do_ioctl(fd_, UI_SET_EVBIT, EV_REL);

		for (int i = 0; i < KEY_MAX; i++)
			do_ioctl(fd_, UI_SET_KEYBIT, i);

		do_ioctl(fd_, UI_SET_KEYBIT, BTN_LEFT);
		do_ioctl(fd_, UI_SET_KEYBIT, BTN_RIGHT);

		do_ioctl(fd_, UI_SET_RELBIT, REL_X);
		do_ioctl(fd_, UI_SET_RELBIT, REL_Y);
		do_ioctl(fd_, UI_SET_RELBIT, REL_WHEEL);
		
		uinput_setup setup;
		memset(&setup, 0, sizeof(setup));
		setup.id.bustype = BUS_USB;
		setup.id.vendor = 1;
		setup.id.product = 1;
		strcpy(setup.name, "kshare");

		do_ioctl(fd_, UI_DEV_SETUP, &setup);
		do_ioctl(fd_, UI_DEV_CREATE);
	} catch (...) {
		close(fd_);
		throw;
	}
}

event_writer::~event_writer() {
	close(fd_);
}

void event_writer::write(const input_event& event) {
	if (::write(fd_, &event, sizeof(event)) != sizeof(event))
		throw std::system_error(std::error_code(errno, std::generic_category()), "Failed to write an event");
}

void event_writer::write(__u16 type, __u16 code, __s32 value) {
	input_event event;	
	event.time.tv_sec = 0;
	event.time.tv_usec = 0;
	event.type = type;
	event.code = code;
	event.value = value;

	write(event);
}

}
}
