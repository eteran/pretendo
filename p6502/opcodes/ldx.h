#ifndef LDX_20121206_H_
#define LDX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_ldx
// Desc: Load X Register
//------------------------------------------------------------------------------
struct opcode_ldx {

	typedef operation_read memory_access;

	static void execute(uint8_t data) {
		X = data;
		update_nz_flags(X);
	}
};

#endif

