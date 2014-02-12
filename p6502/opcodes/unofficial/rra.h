#ifndef RRA_20121206_H_
#define RRA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_rra
// Desc: Rotate Memory Right and then add to A
//------------------------------------------------------------------------------
struct opcode_rra {

	typedef operation_modify memory_access;
	
	void operator()(Context &ctx, uint8_t &data) const {
		// only correct because C_MASK is 0x01, we basically want to shift in an 0x80
		// if C_MASK is set, and a 0 if it is not
		const uint8_t bit = (ctx.P & C_MASK) << 7;
		set_flag_condition<C_MASK>(ctx, data & 0x01);
		data = ((data >> 1) & 0x7f) | bit;
		
		const uint16_t temp16 = ctx.A + data + (ctx.P & C_MASK);
		const bool carry      = (temp16 > 0xff);
		const bool overflow   = ((~(ctx.A ^ data)) & (ctx.A ^ temp16) & 0x80) != 0;
		set_flag_condition<V_MASK>(ctx, overflow);
		set_flag_condition<C_MASK>(ctx, carry);
		ctx.A = temp16 & 0x00ff;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

