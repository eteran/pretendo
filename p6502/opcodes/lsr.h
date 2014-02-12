#ifndef LSR_20121206_H_
#define LSR_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_lsr
// Desc: Logical Shift Right
//------------------------------------------------------------------------------
struct opcode_lsr {

	typedef operation_modify memory_access;
	
	void operator()(Context &ctx, uint8_t &data) const {
		set_flag_condition<C_MASK>(ctx, data & 0x01);
		data >>= 1;
		update_nz_flags(ctx, data);
	}
};

#endif

