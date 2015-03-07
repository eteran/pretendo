
#ifndef SQUARE_20130206_TCC_
#define SQUARE_20130206_TCC_

namespace nes {
namespace apu {

//------------------------------------------------------------------------------
// Name: Square
//------------------------------------------------------------------------------
template <int Channel>
Square<Channel>::Square() : sweep(this), timer_reload_(0), duty_(0),
		sequence_index_(0), enabled_(false) {


	static_assert(Channel >= 0 && Channel < 2, "only channels 0 and 1 are valid");

}

//------------------------------------------------------------------------------
// Name: ~Square
//------------------------------------------------------------------------------
template <int Channel>
Square<Channel>::~Square() {
}

//------------------------------------------------------------------------------
// Name: set_enabled
//------------------------------------------------------------------------------
template <int Channel>
void Square<Channel>::set_enabled(bool value) {
	if(value) {
		enable();
	} else {
		disable();
	}
}

//------------------------------------------------------------------------------
// Name: enable
//------------------------------------------------------------------------------
template <int Channel>
void Square<Channel>::enable() {
	enabled_ = true;
}

//------------------------------------------------------------------------------
// Name: disable
//------------------------------------------------------------------------------
template <int Channel>
void Square<Channel>::disable() {
	enabled_ = false;
	length_counter.clear();
}

//------------------------------------------------------------------------------
// Name: write_reg0
//------------------------------------------------------------------------------
template <int Channel>
void Square<Channel>::write_reg0(uint8_t value) {

	duty_ = (value >> 6) & 0x03;

	if(value & 0x20) {
		length_counter.halt();
	} else {
		length_counter.resume();
	}

	envelope.set_control(value);
}

//------------------------------------------------------------------------------
// Name: write_reg1
//------------------------------------------------------------------------------
template <int Channel>
void Square<Channel>::write_reg1(uint8_t value) {
	sweep.set_control(value);
}

//------------------------------------------------------------------------------
// Name: write_reg2
//------------------------------------------------------------------------------
template <int Channel>
void Square<Channel>::write_reg2(uint8_t value) {

	timer_reload_    = (timer_reload_ & 0xff00) | value;
	timer_.frequency = (timer_reload_ + 1) * 2;
	sweep.set_pulse_period(timer_reload_);
}

//------------------------------------------------------------------------------
// Name: write_reg3
//------------------------------------------------------------------------------
template <int Channel>
void Square<Channel>::write_reg3(uint8_t value) {

	timer_reload_    = (timer_reload_ & 0x00ff) | ((value & 0x07) << 8);
	timer_.frequency = (timer_reload_ + 1) * 2;
	sweep.set_pulse_period(timer_reload_);

	if(enabled_) {
		length_counter.load((value >> 3) & 0x1f);
	}

	sequence_index_ = 0;
	envelope.start();
}

//------------------------------------------------------------------------------
// Name: tick
//------------------------------------------------------------------------------
template <int Channel>
void Square<Channel>::tick() {

	timer_.tick([this]() {
		sequence_index_ = (sequence_index_ + 1) % 8;
	});
}

//------------------------------------------------------------------------------
// Name: enabled
//------------------------------------------------------------------------------
template <int Channel>
bool Square<Channel>::enabled() const {
	return enabled_;
}

//------------------------------------------------------------------------------
// Name: output
//------------------------------------------------------------------------------
template <int Channel>
uint8_t Square<Channel>::output() const {

	static const uint8_t sequence[4][8] = {
		{ 0,1,0,0,0,0,0,0 },
		{ 0,1,1,0,0,0,0,0 },
		{ 0,1,1,1,1,0,0,0 },
		{ 1,0,0,1,1,1,1,1 }
	};

	if((timer_.frequency - 1) < 8) {
		return 0;
	} else if(sequence[duty_][sequence_index_] == 0) {
		return 0;
	} else if(sweep.silenced()) {
		return 0;
	} else if(length_counter.value() == 0) {
		return 0;
	} else {
		return envelope.volume();
	}
}

}
}

#endif
