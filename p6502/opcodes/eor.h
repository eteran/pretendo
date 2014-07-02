#ifndef EOR_20121206_H_
#define EOR_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_eor
// Desc: Exclusive OR
//------------------------------------------------------------------------------
struct opcode_eor {

	typedef operation_read memory_access;

	void operator()(uint8_t data) const {
		A ^= data;
		update_nz_flags(A);
	}
};

#endif

