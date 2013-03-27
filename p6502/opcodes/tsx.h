#ifndef TSX_20121206_H_
#define TSX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tsx
// Desc: Transfer Stack Pointer to X
//------------------------------------------------------------------------------
struct opcode_tsx {
	typedef operation_none memory_access;
	
	void operator()() const {
		X = S;
		update_nz_flags(X);
	}
};

#endif

