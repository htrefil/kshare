#ifndef COMMON_X11_CONTEXT_HPP
#define COMMON_X11_CONTEXT_HPP
#include <cstdint>
#include <utility>
#include <X11/Xlib.h>

namespace common {
namespace x11 {

class context {
public:
	context();

	~context();

	uint16_t width() const;

	uint16_t height() const;

	std::pair<uint16_t, uint16_t> mouse_position();

	void set_mouse_position(std::pair<uint16_t, uint16_t> position);

private:
	Display* display_;
	Window root_window_;
	uint16_t width_;
	uint16_t height_;
};

}
}

#endif