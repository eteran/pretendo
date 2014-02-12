#ifndef XAS_20121206_H_
#define XAS_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_xas
// Desc: AND X register with accumulator and store result in stack
//       pointer, then AND stack pointer with the high byte of the
//       target address of the argument + 1. Store result in memory.
//------------------------------------------------------------------------------
struct opcode_xas {

	typedef operation_write memory_access;
	
	uint8_t operator()(Context &ctx, uint16_t address) const {
		ctx.S = ctx.X & ctx.A;
		return ctx.S & (((address >> 8) + 1) & 0xff);
	}
};

#endif

