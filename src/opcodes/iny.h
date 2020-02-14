#ifndef INY_20121206_H_
#define INY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_iny
// Desc: Increment Y Register
//------------------------------------------------------------------------------
struct opcode_iny {

	using memory_access = operation_none;

	static void execute() {
		update_nz_flags(++Y);
	}
};

#endif
