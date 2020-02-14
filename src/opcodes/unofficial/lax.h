#ifndef LAX_20121206_H_
#define LAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_lax
// Desc: Load A/X
//------------------------------------------------------------------------------
struct opcode_lax {

	using memory_access = operation_read;

	static void execute(uint8_t data) {
		A = data;
		X = data;
		update_nz_flags(A);
	}
};

#endif
