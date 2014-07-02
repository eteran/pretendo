
#ifndef AXA_20121206_H_
#define AXA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_axa
// Desc: AND X register with accumulator then AND result with 7 and store in memory.
//------------------------------------------------------------------------------
struct opcode_axa {

	typedef operation_write memory_access;

	uint8_t operator()(uint16_t address) const {
		(void)address;
		X &= A;
		X &= 7;
		return X;
	}
};

#endif

