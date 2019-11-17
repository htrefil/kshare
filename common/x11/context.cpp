#include <cassert>
#include <stdexcept>

#include "context.hpp"

namespace common {
namespace x11 {

context::context() {
	display_ = XOpenDisplay(nullptr);
	if (display_ == nullptr)
		throw std::runtime_error("Failed to open display");

	try {
		auto count = XScreenCount(display_);
		if (count > 1)
			throw std::runtime_error("More than one screen has been found");

		root_window_ = XRootWindow(display_, 0);

		// Get screen size.
		XWindowAttributes attributes;
		if (XGetWindowAttributes(display_, root_window_, &attributes) == 0)
			throw std::runtime_error("Failed to get window attributes");

		assert(attributes.width <= 0xFFFF && attributes.height <= 0xFFFF);

		width_ = (uint16_t)attributes.width;
		height_ = (uint16_t)attributes.height;
	} catch (...) {
		XCloseDisplay(display_);
		throw;
	}
}

context::~context() {
	XCloseDisplay(display_);
}

uint16_t context::width() const {
	return width_;
}

uint16_t context::height() const {
	return height_;
}

std::pair<uint16_t, uint16_t> context::mouse_position() {
	int root_x;
	int root_y;
	int win_x;
	int win_y;
	unsigned int mask;
	Window rwindow;
	if (!XQueryPointer(display_, root_window_, &rwindow, &rwindow, &root_x, &root_y, &win_x, &win_y, &mask))
		throw std::runtime_error("Failed to get mouse position");

	assert(root_x <= 0xFFFF && root_y <= 0xFFFF);

	return { (uint16_t)root_x, (uint16_t)root_y };
}

void context::set_mouse_position(std::pair<uint16_t, uint16_t> position) {
	auto [x, y] = position;

	XSelectInput(display_, root_window_, KeyReleaseMask);
	XWarpPointer(display_, None, root_window_, 0, 0, 0, 0, x, y);
	XFlush(display_);
}

}
}