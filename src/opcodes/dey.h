#ifndef DEY_20121206_H_
#define DEY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_dey
// Desc: Decrement Y Register
//------------------------------------------------------------------------------
struct opcode_dey {

	using memory_access = operation_none;

	static void execute() {
		update_nz_flags(--Y);
	}
};

#endif
