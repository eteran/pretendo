
#include "LengthCounter.h"
#include "NES.h"

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
	0x20, 0x1e
};
}

//------------------------------------------------------------------------------
// Name: LengthCounter
//------------------------------------------------------------------------------
LengthCounter::LengthCounter() : halt_cycle_(0), reload_cycle_(0), reload_value_(0), value_(0), halt_(false), prev_halt_(false), reload_(false) {

}

//------------------------------------------------------------------------------
// Name: ~LengthCounter
//------------------------------------------------------------------------------
LengthCounter::~LengthCounter() {

}

//------------------------------------------------------------------------------
// Name: load
//------------------------------------------------------------------------------
void LengthCounter::load(uint8_t index) {

	// still fails: 11.len_reload_timing.nes
	if(reload_) {
		value_  = reload_value_;
		reload_ = false;
	}
	
	reload_value_ = length_table[index & 0x1f];
	reload_       = true;
	reload_cycle_ = nes::apu.cycle_count();
}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void LengthCounter::reset() {
	prev_halt_    = false;
	halt_         = false;
	reload_       = false;
	value_        = 0;
	reload_value_ = 0;
	reload_cycle_ = 0;
	halt_cycle_   = 0;
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
	halt_cycle_ = nes::apu.cycle_count();
}

//------------------------------------------------------------------------------
// Name: resume
//------------------------------------------------------------------------------
void LengthCounter::resume() {
	prev_halt_  = halt_;
	halt_       = false;
	halt_cycle_ = nes::apu.cycle_count();
}

//------------------------------------------------------------------------------
// Name: value
//------------------------------------------------------------------------------
uint8_t LengthCounter::value()  {
	if(reload_) {
		value_  = reload_value_;
		reload_ = false;
	}

	return value_;
}

//------------------------------------------------------------------------------
// Name: clock
//------------------------------------------------------------------------------
uint8_t LengthCounter::clock() {

	// reload the value if needed
	if(reload_) {	
		if((reload_cycle_ != nes::apu.cycle_count()) || (value_ == 0)) {
			value_ = reload_value_;
		}
		reload_ = false;
	}
	
	bool halted;
	
	// delay the halt 1 cycle
	if(halt_cycle_ == nes::apu.cycle_count()) {
		halted = prev_halt_;
	} else {
		halted = halt_;
	}

	if(!halted && value_ > 0) {
		--value_;
	}

	
	return value_;
}
