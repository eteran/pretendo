#ifndef TYA_20121206_H_
#define TYA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tya
// Desc: Transfer Y to A
//------------------------------------------------------------------------------
struct opcode_tya {
	typedef operation_none memory_access;
	
	static void execute() {
		A = Y;
		update_nz_flags(A);
	}
};

#endif

