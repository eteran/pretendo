
#include "Sweep.h"
#include "Timer.h"

//------------------------------------------------------------------------------
// Name: Sweep
//------------------------------------------------------------------------------
Sweep::Sweep(Timer &timer) : timer_(timer), value_(0), pulse_period_(0),
		counter_(0), period_(0), shift_(0), negate_(0), enable_(0),
		reload_(false), silenced_(false) {

}

//------------------------------------------------------------------------------
// Name: ~Sweep
//------------------------------------------------------------------------------
Sweep::~Sweep() {

}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
void Sweep::set_pulse_period(uint16_t value) {
	pulse_period_ = value;
}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
uint16_t Sweep::target_period(int channel) const {

	// The channel's 11-bit raw timer period is shifted right by the shift count 
	// (using a barrel shifter), then either added to or subtracted from the 
	// channel's raw period, yielding the target period.
	const uint16_t delta = pulse_period_ >> shift_;

	if(negate_) {
		if(channel == 0) {
			return pulse_period_ - delta - 1;
		} else {
			return pulse_period_ - delta;
		}
	} else {
		return pulse_period_ + delta;
	}
}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
void Sweep::clock(int channel) {
		
	if(counter_) {
		if(--counter_ == 0) {		
			const uint16_t target = target_period(channel);

			if(pulse_period_ < 8 || target > 0x7ff) {

				// When the channel's current period is less than 8 or the target period 
				// is greater than $7FF, the channel is silenced (0 is sent to the mixer) 
				// but the channel's current period remains unchanged.
				silenced_ = true;
			} else {

				// Otherwise, if the enable flag is set and the shift count is non-zero, 
				// when the divider outputs a clock, the channel's period is updated.
				silenced_ = false;
				if(enable_ && shift_) {
					timer_.set_frequency((target + 1) * 2);
				}
			}

			if(reload_) {
				reload_  = false;		
				counter_ = period_ + 1;
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
void Sweep::set_control(uint8_t value) {
	enable_ = (value & 0x80);
	period_ = (value >> 4) & 0x07;
	negate_ = (value & 0x08);
	shift_  = (value & 0x07);
}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
void Sweep::reload() {
	reload_ = true;
}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
bool Sweep::silenced() const {
	return silenced_;
}
