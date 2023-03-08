#ifndef XAS_20121206_H_
#define XAS_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_xas
// Desc: AND X register with accumulator and store result in stack
//       pointer, then AND stack pointer with the high byte of the
//       target address of the argument + 1. Store result in memory.
//------------------------------------------------------------------------------
struct opcode_xas {

	using memory_access = operation_write;

	static uint8_t execute(uint32_t address) {
		S = X & A;
		return S & (((address >> 8) + 1) & 0xff);
	}
};

#endif
