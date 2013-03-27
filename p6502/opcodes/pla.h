#ifndef PLA_20121206_H_
#define PLA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_pla
// Desc: Pull Accumulator
//------------------------------------------------------------------------------
struct opcode_pla {

	typedef operation_stack_read memory_access;

	void operator()(uint8_t data) const {
		A = data;
		update_nz_flags(A);
	}
};

#endif

