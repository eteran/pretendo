#ifndef STY_20121206_H_
#define STY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sty
// Desc: Store Y Register
//------------------------------------------------------------------------------
struct opcode_sty {

	typedef operation_write memory_access;

	static uint8_t execute(uint16_t address) {
		(void)address;
		return Y;
	}
};


#endif

