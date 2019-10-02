#ifndef LDY_20121206_H_
#define LDY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_ldy
// Desc: Load Y Register
//------------------------------------------------------------------------------
struct opcode_ldy {

	using memory_access = operation_read;

	static void execute(uint8_t data) {
		Y = data;
		update_nz_flags(Y);
	}
};

#endif

