#ifndef TYA_20121206_H_
#define TYA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tya
// Desc: Transfer Y to Accumulator
//------------------------------------------------------------------------------
struct opcode_tya {
	typedef operation_none memory_access;
	
	void operator()() const {
		A = Y;
		update_nz_flags(A);
	}
};

#endif

