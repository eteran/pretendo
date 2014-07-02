#ifndef AND_20121206_H_
#define AND_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_and
// Desc: Logical AND
//------------------------------------------------------------------------------
struct opcode_and {

	typedef operation_read memory_access;

	void operator()(uint8_t data) const {
		A &= data;
		update_nz_flags(A);
	}
};

#endif

