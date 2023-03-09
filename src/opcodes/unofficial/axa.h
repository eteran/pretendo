
#ifndef AXA_20121206_H_
#define AXA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_axa
// Desc: AND X register with accumulator then AND result with 7 and store in memory.
//------------------------------------------------------------------------------
struct opcode_axa {

	using memory_access = operation_write;

	static uint8_t execute(uint_least16_t address) {
		(void)address;
		X &= A;
		X &= 7;
		return X;
	}
};

#endif
