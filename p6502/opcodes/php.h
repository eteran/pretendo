#ifndef PHP_20121206_H_
#define PHP_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_php
// Desc: Push Processor Status
//------------------------------------------------------------------------------
struct opcode_php {

	using memory_access = operation_stack_write;

	static uint8_t execute() {
		return P | B_MASK | R_MASK;
	}
};

#endif

