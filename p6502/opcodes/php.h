#ifndef PHP_20121206_H_
#define PHP_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_php
// Desc: Push Processor Status
//------------------------------------------------------------------------------
struct opcode_php {

	typedef operation_stack_write memory_access;

	uint8_t operator()(Context &ctx) const {
		return ctx.P | B_MASK | R_MASK;
	}
};

#endif

