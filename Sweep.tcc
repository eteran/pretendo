
#ifndef SWEEP_20130502_TCC_
#define SWEEP_20130502_TCC_

#include "Timer.h"
#include "Square.h"

//------------------------------------------------------------------------------
// Name: Sweep
//------------------------------------------------------------------------------
template <int Channel>
Sweep<Channel>::Sweep(Square<Channel> *square) : square_(square),  
		pulse_period_(0), counter_(0), control_(0), reload_(false), 
		silenced_(false) {

}

//------------------------------------------------------------------------------
// Name: ~Sweep
//------------------------------------------------------------------------------
template <int Channel>
Sweep<Channel>::~Sweep() {
}

//------------------------------------------------------------------------------
// Name: set_pulse_period
//------------------------------------------------------------------------------
template <int Channel>
void Sweep<Channel>::set_pulse_period(uint16_t value) {
	pulse_period_ = value;
}

//------------------------------------------------------------------------------
// Name: target_period
//------------------------------------------------------------------------------
template <int Channel>
uint16_t Sweep<Channel>::target_period() const {

	// The channel's 11-bit raw timer period is shifted right by the shift count
	// (using a barrel shifter), then either added to or subtracted from the
	// channel's raw period, yielding the target period.
	const uint16_t delta = pulse_period_ >> shift();

	if(negate()) {
		if(Channel == 0) {
			return pulse_period_ - delta - 1;
		} else {
			return pulse_period_ - delta;
		}
	} else {
		return pulse_period_ + delta;
	}
}

//------------------------------------------------------------------------------
// Name: clock
//------------------------------------------------------------------------------
template <int Channel>
void Sweep<Channel>::clock() {

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
// Name: enabled
//------------------------------------------------------------------------------
template <int Channel>
bool Sweep<Channel>::enabled() const {
	return control_ & 0x80;
}

//------------------------------------------------------------------------------
// Name: negate
//------------------------------------------------------------------------------
template <int Channel>
bool Sweep<Channel>::negate() const {
	return control_ & 0x08;
}

//------------------------------------------------------------------------------
// Name: period
//------------------------------------------------------------------------------
template <int Channel>
uint8_t Sweep<Channel>::period() const {
	return (control_ >> 4) & 0x07;
}

//------------------------------------------------------------------------------
// Name: shift
//------------------------------------------------------------------------------
template <int Channel>
uint8_t Sweep<Channel>::shift() const {
	return control_ & 0x07;
}

//------------------------------------------------------------------------------
// Name: set_control
//------------------------------------------------------------------------------
template <int Channel>
void Sweep<Channel>::set_control(uint8_t value) {	
	control_ = value;
	reload_  = true;
}

//------------------------------------------------------------------------------
// Name: silenced
//------------------------------------------------------------------------------
template <int Channel>
bool Sweep<Channel>::silenced() const {
	return silenced_;
}

#endif
