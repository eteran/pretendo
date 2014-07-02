#ifndef STY_20121206_H_
#define STY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sty
// Desc: Store Y Register
//------------------------------------------------------------------------------
struct opcode_sty {

	typedef operation_write memory_access;

	uint8_t operator()(uint16_t address) const {
		(void)address;
		return Y;
	}
};


#endif

