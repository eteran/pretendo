
#include "Sweep.h"
#include "Timer.h"

//------------------------------------------------------------------------------
// Name: Sweep
//------------------------------------------------------------------------------
Sweep::Sweep(Timer &timer) : timer_(timer), value_(0), target_period_(0), 
		divider_(0), divider_period_(0), shift_(0), negate_(0), enable_(0),
		reload_(false), silence_(false) {

}

//------------------------------------------------------------------------------
// Name: ~Sweep
//------------------------------------------------------------------------------
Sweep::~Sweep() {

}

//------------------------------------------------------------------------------
// Name: update_target_period
//------------------------------------------------------------------------------
void Sweep::update_target_period(bool from_divider) {
	
	
	// The channel's 11-bit raw timer period is shifted right by the shift count 
	// (using a barrel shifter), then either added to or subtracted from the 
	// channel's raw period, yielding the target period.
	const uint16_t timer_value = timer_.value() >> shift_;
	if(negate_) {
		// TODO: implement one's compliment here for Square 1
		target_period_ = timer_value - timer_.value();
	} else {
		target_period_ = timer_value + timer_.value();
	}
	

	if(timer_.frequency() < 8 || target_period_ > 0x7ff) {
		// When the channel's current period is less than 8 or the target period 
		// is greater than $7FF, the channel is silenced (0 is sent to the mixer) 
		// but the channel's current period remains unchanged.
		silence_ = true;
	} else {
		// Otherwise, if the enable flag is set and the shift count is non-zero, 
		// when the divider outputs a clock, the channel's period is updated.
	
		silence_ = false;
		if(enable_ && shift_ && from_divider) {
			timer_.set_frequency(target_period_);
		}
	}
	
	
}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
void Sweep::clock() {

	if(divider_) {	
		--divider_;
		
		if(!divider_) {
			update_target_period(true);
		
		
			if(reload_) {
				reload_  = false;		
				divider_ = divider_period_ + 1;
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
void Sweep::clock_divider() {
		
	if(divider_ == 0) {
		divider_ = divider_period_;
	}
}

//------------------------------------------------------------------------------
// Name: 
//------------------------------------------------------------------------------
void Sweep::set_control(uint8_t value) {
	enable_         = (value & 0x80);
	divider_period_ = (value >> 4) & 0x07;
	negate_         = (value & 0x08);
	shift_          = (value & 0x07);
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
bool Sweep::silence_channel() const {
	return silence_;
}
