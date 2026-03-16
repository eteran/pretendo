#ifndef AAX_20121206_H_
#define AAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_aax
// Desc: Store A & X
//------------------------------------------------------------------------------
struct opcode_aax {

	using memory_access = operation_write;

	static uint8_t execute([[maybe_unused]] uint_least16_t address) {
		return A & X;
	}
};

#endif
