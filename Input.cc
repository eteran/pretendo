
#include "Input.h"
#include <iostream>

//------------------------------------------------------------------------------
// Name: Input
//------------------------------------------------------------------------------
Input::Input() : strobe_(0) {
}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void Input::reset() {
	strobe_ = 0;
	std::cout << "[Input::reset] reset complete" << std::endl;
}

//------------------------------------------------------------------------------
// Name: write4016
//------------------------------------------------------------------------------
void Input::write4016(uint8_t value) {

	/*
	The 4021 has inverting inputs.
	Every time the Strobe (STRB) connection gets a HIGH then LOW signal,
	the data on the 8 parrallel inputs gets latched in the register.
	Since the inputs of the 4021 are inverting,
	the 8 resistors act as pull-up resistors to the +5V making the input HIGH by default.
	When a button is pressed, 0V is allowed to the input, thus enabling the state in the register.
	The serial data input is tied to 0V so as not to act upon the internal flip-flops of the 4021.
	Then, everytime the Clock (CLK) connection gets a HIGH signal,
	rightmost bit (P8) is sent to D0 through Q8. After 8 shift,
	the entire register is zero and thus all other read return a 0 state.
	*/

	value &= 0x1;

	if(!value && strobe_) {
		controller1_.poll();
	}

	strobe_ = value;
}

//------------------------------------------------------------------------------
// Name: read4016
//------------------------------------------------------------------------------
uint8_t Input::read4016() {
	return controller1_.read() | 0x40;
}

//------------------------------------------------------------------------------
// Name: read4017
//------------------------------------------------------------------------------
uint8_t Input::read4017() {
	return 0x40;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Controller &Input::controller1() {
	return controller1_;
}
