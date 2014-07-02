#ifndef TXA_20121206_H_
#define TXA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_txa
// Desc: Transfer X to A
//------------------------------------------------------------------------------
struct opcode_txa {
	typedef operation_none memory_access;
	
	void operator()() const {
		A = X;
		update_nz_flags(A);
	}
};

#endif

