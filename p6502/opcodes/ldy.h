#ifndef LDY_20121206_H_
#define LDY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_ldy
// Desc: Load Y Register
//------------------------------------------------------------------------------
struct opcode_ldy {

	typedef operation_read memory_access;

	void operator()(uint8_t data) const {
		Y = data;
		update_nz_flags(Y);
	}
};

#endif

