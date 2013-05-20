
#include "Sweep.h"
#include "Timer.h"
#include "Square.h"

//------------------------------------------------------------------------------
// Name: Sweep
//------------------------------------------------------------------------------
Sweep::Sweep(int channel, Square *square) : square_(square), channel_(channel), 
		pulse_period_(0), counter_(0), control_(0), reload_(false), 
		silenced_(false) {

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
uint16_t Sweep::target_period() const {

	// The channel's 11-bit raw timer period is shifted right by the shift count
	// (using a barrel shifter), then either added to or subtracted from the
	// channel's raw period, yielding the target period.
	const uint16_t delta = pulse_period_ >> shift();

	if(negate()) {
		if(channel_ == 0) {
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
void Sweep::clock() {

	// If the reload flag flag is set, the divider's counter is set to the
	// period P. If the divider's counter was zero before the reload, the
	// pulse's period is also adjusted.
	if(reload_) {
		reload_ = false;
		const uint8_t prev_counter = counter_;
		counter_ = period() + 1;

		if(prev_counter == 0 && enabled() && shift()) {
			const uint16_t target = target_period();
			if(target < 0x800) {
				square_->timer_reload_ = target;
				pulse_period_          = target;
				square_->timer_.set_frequency((target + 1) * 2);
			}
		}

	} else {
		if(counter_) {
			// If the reload flag is clear and the divider's counter is non-zero,
			// it is decremented.
			--counter_;
		} else {
			// If the reload flag is clear and the divider's counter is zero,
			// the counter is set to P and the pulse's period is adjusted.
			counter_ = period() + 1;

			if(enabled() && shift()) {
				const uint16_t target = target_period();
				if(target < 0x800) {
					square_->timer_reload_ = target;
					pulse_period_          = target;
					square_->timer_.set_frequency((target + 1) * 2);
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool Sweep::enabled() const {
	return control_ & 0x80;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool Sweep::negate() const {
	return control_ & 0x08;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Sweep::period() const {
	return (control_ >> 4) & 0x07;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Sweep::shift() const {
	return control_ & 0x07;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Sweep::set_control(uint8_t value) {	
	control_ = value;
	reload_  = true;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool Sweep::silenced() const {
	return silenced_;
}
