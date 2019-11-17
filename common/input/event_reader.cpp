#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <cerrno>
#include <filesystem>
#include <system_error>

#include "event_reader.hpp"

namespace common {
namespace input {

event_reader::event_reader(callback_func callback, int timeout) : callback_(callback), timeout_(timeout) {
	try {
		for (const auto& entry : std::filesystem::directory_iterator("/dev/input/")) {
			if (entry.is_directory())
				continue;

			auto fd = open(entry.path().c_str(), O_RDONLY);
			if (fd == -1)
				throw std::system_error(std::error_code(errno, std::generic_category()), "Failed to open an input");

			fds_.push_back(pollfd { fd, POLLIN, 0 });
			buffers_.push_back(buffer { input_event(), 0 });
		}

		// Need to wait a while.
		sleep(1);

		for (size_t i = 0; i < fds_.size(); i++) {
			auto fd = fds_[i].fd;

			if(ioctl(fd, EVIOCGRAB, 1) != 0) {
				auto err = errno;
				close(fd);

				// Happens with some devices that are probably not meant to be used this way.
				if (err == ENOTTY) {
					fds_.erase(fds_.begin() + i);
					buffers_.erase(buffers_.begin() + i);

					continue;
				}

				throw std::system_error(std::error_code(err, std::generic_category()), "Failed to do ioctl");
			}
		}
	} catch (...) {
		cleanup();
		throw;
	}
}

void event_reader::process() {
	if (poll(fds_.data(), fds_.size(), timeout_) == -1)
		throw std::system_error(std::error_code(errno, std::generic_category()), "Failed to poll inputs");

	for (size_t i = 0; i < fds_.size(); i++) {
		const auto& fd = fds_[i];
		auto& buffer = buffers_[i];

		if (fd.revents == fd.events) {
			auto size = read(fd.fd, (unsigned char*)&buffer.event_ + buffer.size_, sizeof(buffer.event_) - buffer.size_);
			if (size == -1)
				throw std::system_error(std::error_code(errno, std::generic_category()), "Failed read from input");

			buffer.size_ += size;
		}

		if (buffer.size_ == sizeof(buffer.event_)) {
			callback_(buffer.event_);
			buffer.size_ = 0;
		}
	}
}

void event_reader::cleanup() {
	for (const auto& fd : fds_)
		close(fd.fd);
}

}
}

