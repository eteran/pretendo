#ifndef STX_20121206_H_
#define STX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_stx
// Desc: Store X Register
//------------------------------------------------------------------------------
struct opcode_stx {

	using memory_access = operation_write;

	static uint8_t execute(uint16_t address) {
		(void)address;
		return X;
	}
};

#endif
