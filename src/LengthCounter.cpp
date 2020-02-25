
#include "LengthCounter.h"
#include "Apu.h"

namespace nes {
namespace apu {

namespace {
const uint8_t length_table[32] = {
	0x0a, 0xfe,
	0x14, 0x02,
	0x28, 0x04,
	0x50, 0x06,
	0xa0, 0x08,
	0x3c, 0x0a,
	0x0e, 0x0c,
	0x1a, 0x0e,
	0x0c, 0x10,
	0x18, 0x12,
	0x30, 0x14,
	0x60, 0x16,
	0xc0, 0x18,
	0x48, 0x1a,
	0x10, 0x1c,
	0x20, 0x1e};
}

//------------------------------------------------------------------------------
// Name: load
//------------------------------------------------------------------------------
void LengthCounter::load(uint8_t index) {

	if (reload_) {
		value_ = reload_value_;
	}

	reload_value_ = length_table[index & 0x1f];
	reload_       = true;
	reload_cycle_ = nes::apu::cycle_count();
}

//------------------------------------------------------------------------------
// Name: clear
//------------------------------------------------------------------------------
void LengthCounter::clear() {
	reload_value_ = 0;
	reload_       = true;
}

//------------------------------------------------------------------------------
// Name: halt
//------------------------------------------------------------------------------
void LengthCounter::halt() {
	prev_halt_  = halt_;
	halt_       = true;
	halt_cycle_ = nes::apu::cycle_count();
}

//------------------------------------------------------------------------------
// Name: resume
//------------------------------------------------------------------------------
void LengthCounter::resume() {
	prev_halt_  = halt_;
	halt_       = false;
	halt_cycle_ = nes::apu::cycle_count();
}

//------------------------------------------------------------------------------
// Name: value
//------------------------------------------------------------------------------
uint8_t LengthCounter::value() const {
	if (reload_) {
		value_  = reload_value_;
		reload_ = false;
	}

	return value_;
}

//------------------------------------------------------------------------------
// Name: clock
//------------------------------------------------------------------------------
void LengthCounter::clock() {

	bool prevent_decrement       = false;
	const uint64_t current_cycle = nes::apu::cycle_count();

	if (reload_) {
		if (reload_cycle_ == current_cycle && value_ == 0) {
			value_            = reload_value_;
			prevent_decrement = true;
		} else if (reload_cycle_ == current_cycle && value_ != 0) {
			// no reload!
		} else {
			value_ = reload_value_;
		}
	}

	if (!prevent_decrement) {
		bool halted;

		// delay the halt 1 cycle
		if (halt_cycle_ == current_cycle) {
			halted = prev_halt_;
		} else {
			halted = halt_;
		}

		if (!halted && value_ > 0) {
			--value_;
		}
	}

	reload_ = false;
}

}
}
