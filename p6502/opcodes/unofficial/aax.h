#ifndef AAX_20121206_H_
#define AAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_aax
// Desc: Store A & X
//------------------------------------------------------------------------------
struct opcode_aax {

	typedef operation_write memory_access;

	static uint8_t execute(uint16_t address) {
		(void)address;
		return A & X;
	}
};


#endif

