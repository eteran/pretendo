
#include "Triangle.h"


//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Triangle::Triangle() : enabled_(false), linear_counter_(0), linear_counter_control_(0), linear_counter_reload_(0), linear_counter_halt_(false) {

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Triangle::~Triangle() {

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
	length_counter_.clear();
}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void Triangle::reset() {

	/*
	length_counter_.reset();
	enabled_ = false;
	linear_counter_ 		= 0;
	linear_counter_control_ = 0;
	linear_counter_reload_  = 0;
	linear_counter_halt_	= false;
	*/
}

//------------------------------------------------------------------------------
// Name: write_reg0
//------------------------------------------------------------------------------
void Triangle::write_reg0(uint8_t value) {

	if(value & 0x80) {
		length_counter_.halt();
	} else {
		length_counter_.resume();
	}
	
	linear_counter_control_ = (value & 0x80);
	linear_counter_reload_  = (value & 0x7f);
}

//------------------------------------------------------------------------------
// Name: write_reg2
//------------------------------------------------------------------------------
void Triangle::write_reg2(uint8_t value) {
	linear_counter_ = (linear_counter_ & 0xff00) | value;
}

//------------------------------------------------------------------------------
// Name: write_reg3
//------------------------------------------------------------------------------
void Triangle::write_reg3(uint8_t value) {

	if(enabled_) {
		length_counter_.load((value >> 3) & 0x1f);
	}
	
	linear_counter_      = (linear_counter_ & 0x00ff) | ((value & 0x07) << 8);
	linear_counter_halt_ = true;
}

//------------------------------------------------------------------------------
// Name: enabled
//------------------------------------------------------------------------------
bool Triangle::enabled() const {
	return enabled_;
}

//------------------------------------------------------------------------------
// Name: length_counter
//------------------------------------------------------------------------------
LengthCounter &Triangle::length_counter() {
	return length_counter_;
}

//------------------------------------------------------------------------------
// Name: tick
//------------------------------------------------------------------------------
void Triangle::tick() {

}
