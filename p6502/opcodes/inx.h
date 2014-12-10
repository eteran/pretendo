#ifndef INX_20121206_H_
#define INX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_inx
// Desc: Increment X Register
//------------------------------------------------------------------------------
struct opcode_inx {

	typedef operation_none memory_access;

	static void execute() {
		update_nz_flags(++X);
	}
};


#endif

