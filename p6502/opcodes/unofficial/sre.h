#ifndef SRE_20121206_H_
#define SRE_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sre
// Desc: Logical Shift Right
//------------------------------------------------------------------------------
struct opcode_sre {

	typedef operation_modify memory_access;
	
	void operator()(uint8_t &data) const {
		set_flag_condition<C_MASK>(data & 0x01);
		data >>= 1;
		A ^= data;
		update_nz_flags(A);
	}
};

#endif

