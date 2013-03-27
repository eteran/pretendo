#ifndef TAX_20121206_H_
#define TAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tax
// Desc: Transfer Accumulator to X
//------------------------------------------------------------------------------
struct opcode_tax {
	typedef operation_none memory_access;
	
	void operator()() const {
		X = A;
		update_nz_flags(X);
	}
};

#endif

