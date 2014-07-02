
#ifndef ASL_20121206_H_
#define ASL_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_asl
// Desc: Arithmetic Shift Left
//------------------------------------------------------------------------------
struct opcode_asl {

	typedef operation_modify memory_access;
	
	void operator()(uint8_t &data) const {
		set_flag_condition<C_MASK>(data & 0x80);
		data <<= 1;
		update_nz_flags(data);
	}
};

#endif

