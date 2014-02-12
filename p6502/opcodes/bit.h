
#ifndef BIT_20121206_H_
#define BIT_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_bit
// Desc: Bit Test
//------------------------------------------------------------------------------
struct opcode_bit {

	typedef operation_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
		set_flag_condition<Z_MASK>(ctx, !(data & ctx.A));
		set_flag_condition<N_MASK>(ctx, data & N_MASK);
		set_flag_condition<V_MASK>(ctx, data & V_MASK);
	}
};

#endif

