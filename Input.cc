
#include "Input.h"
#include "Controller.h"
#include "NES.h"
#include <iostream>

namespace nes {
namespace input {

uint8_t    strobe_      = 0;
Controller controller1;
Controller controller2;
Controller controller3;
Controller controller4;


/*
 * Notes about the Four Score:
 * The read procedure looks like this:
 *
 * Read $4016 8 times to get the 8 buttons for controller #1.
 * Read $4016 8 more times to get the buttons for controller #3.
 * Read $4016 8 more times to get the signature, $10.
 * Read $4017 8 times to get the 8 buttons for controller #2.
 * Read $4017 8 more times to get the buttons for controller #4.
 * Read $4017 8 more times to get the signature, $20.
 *
 * The $10 and $20 signatures are sent most significant bit first.
 * They can be used to detect 1. whether the Four Score switch is in the
 * 4 player position and 2. whether a DMC sample fetch caused a bit deletion.
 */

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void reset() {
	strobe_ = 0;
}

//------------------------------------------------------------------------------
// Name: write4016
//------------------------------------------------------------------------------
void write4016(uint8_t value) {

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
		controller1.poll();
		controller2.poll();
		controller3.poll();
		controller4.poll();
	}

	strobe_ = value;
}

//------------------------------------------------------------------------------
// Name: read4016
//------------------------------------------------------------------------------
uint8_t read4016() {
	// The ouput looks like this for NES:
	// OOOxxxxD
	// Where:
	// O = open bus
	// x = zero
	// D = data
	return (controller1.read() & 0x1f) | 0x40;
}

//------------------------------------------------------------------------------
// Name: read4017
//------------------------------------------------------------------------------
uint8_t read4017() {
	// The ouput looks like this for NES:
	// OOOxxxxD
	// Where:
	// O = open bus
	// x = zero
	// D = data
	return (controller2.read() & 0x1f) | 0x40;
}

}
}
