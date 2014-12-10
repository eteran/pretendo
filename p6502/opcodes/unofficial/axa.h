
#ifndef AXA_20121206_H_
#define AXA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_axa
// Desc: AND X register with accumulator then AND result with 7 and store in memory.
//------------------------------------------------------------------------------
struct opcode_axa {

	typedef operation_write memory_access;

	static uint8_t execute(uint16_t address) {
		(void)address;
		X &= A;
		X &= 7;
		return X;
	}
};

#endif

