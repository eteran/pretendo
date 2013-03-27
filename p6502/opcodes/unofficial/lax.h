#ifndef LAX_20121206_H_
#define LAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_lax
// Desc: Load Accumulator/X
//------------------------------------------------------------------------------
struct opcode_lax {

	typedef operation_read memory_access;

	void operator()(uint8_t data) const {
		A = data;
		X = data;
		update_nz_flags(A);
	}
};

#endif

