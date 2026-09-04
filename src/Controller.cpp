
#include "Controller.h"

//------------------------------------------------------------------------------
// Name: read
//------------------------------------------------------------------------------
uint8_t Controller::read() {
	if (read_index_++ < 8) {
		return data_.read();
	} else {
		return 0x01;
	}
}

//------------------------------------------------------------------------------
// Name: poll
//------------------------------------------------------------------------------
void Controller::poll() {

	if (connected_) {
		data_.load(keystate_.load(std::memory_order_relaxed));
		read_index_ = 0;
	}
}

//------------------------------------------------------------------------------
// Name: set_button
//------------------------------------------------------------------------------
void Controller::set_button(int index, bool pressed) {

	const auto mask = static_cast<uint8_t>(1u << index);

	if (pressed) {
		keystate_.fetch_or(mask, std::memory_order_relaxed);
	} else {
		keystate_.fetch_and(static_cast<uint8_t>(~mask), std::memory_order_relaxed);
	}
}

//------------------------------------------------------------------------------
// Name: clear_buttons
//------------------------------------------------------------------------------
void Controller::clear_buttons() {
	keystate_.store(0, std::memory_order_relaxed);
}
