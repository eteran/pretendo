#ifndef INX_20121206_H_
#define INX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_inx
// Desc: Increment X Register
//------------------------------------------------------------------------------
struct opcode_inx {

	using memory_access = operation_none;

	static void execute() {
		update_nz_flags(++X);
	}
};


#endif

