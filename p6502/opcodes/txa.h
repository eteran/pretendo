#ifndef TXA_20121206_H_
#define TXA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_txa
// Desc: Transfer X to A
//------------------------------------------------------------------------------
struct opcode_txa {
	using memory_access = operation_none;
	
	static void execute() {
		A = X;
		update_nz_flags(A);
	}
};

#endif

