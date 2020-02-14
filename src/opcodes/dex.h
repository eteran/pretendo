#ifndef DEX_20121206_H_
#define DEX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_dex
// Desc: Decrement X Register
//------------------------------------------------------------------------------
struct opcode_dex {

	using memory_access = operation_none;

	static void execute() {
		update_nz_flags(--X);
	}
};

#endif
