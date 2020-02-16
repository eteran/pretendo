
#include "Triangle.h"

namespace nes {
namespace apu {
namespace {

const uint8_t sequence[32] = {
	0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Triangle::set_enabled(bool value) {
	if (value) {
		enable();
	} else {
		disable();
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Triangle::enable() {
	enabled_ = true;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Triangle::disable() {
	enabled_ = false;
	length_counter.clear();
}

//------------------------------------------------------------------------------
// Name: write_reg0
//------------------------------------------------------------------------------
void Triangle::write_reg0(uint8_t value) {

	if (value & 0x80) {
		length_counter.halt();
	} else {
		length_counter.resume();
	}

	linear_counter.set_control(value);
}

//------------------------------------------------------------------------------
// Name: write_reg2
//------------------------------------------------------------------------------
void Triangle::write_reg2(uint8_t value) {
	timer_load_      = (timer_load_ & 0xff00) | value;
	timer_.frequency = (timer_load_ + 1);
}

//------------------------------------------------------------------------------
// Name: write_reg3
//------------------------------------------------------------------------------
void Triangle::write_reg3(uint8_t value) {

	if (enabled_) {
		length_counter.load((value >> 3) & 0x1f);
	}

	timer_load_      = (timer_load_ & 0x00ff) | ((value & 0x07) << 8);
	timer_.frequency = (timer_load_ + 1);

	linear_counter.reload();
}

//------------------------------------------------------------------------------
// Name: enabled
//------------------------------------------------------------------------------
bool Triangle::enabled() const {
	return enabled_;
}

//------------------------------------------------------------------------------
// Name: tick
//------------------------------------------------------------------------------
void Triangle::tick() {

	timer_.tick([this]() {
		if (length_counter.value() && linear_counter.value()) {
			sequence_index_ = (sequence_index_ + 1) % 32;
		}
	});
}

//------------------------------------------------------------------------------
// Name: dac
//------------------------------------------------------------------------------
uint8_t Triangle::output() const {
	if (timer_.frequency < 4) {
		return 0x00;
	} else {
		return sequence[sequence_index_];
	}
}

}
}
