#ifndef PHA_20121206_H_
#define PHA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_pha
// Desc: Push Accumulator
//------------------------------------------------------------------------------
struct opcode_pha {

	typedef operation_stack_write memory_access;

	static uint8_t execute() {
		return A;
	}
};

#endif

