#ifndef ADC_20121206_H_
#define ADC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_adc
// Desc: Add with Carry
//------------------------------------------------------------------------------
struct opcode_adc {

	typedef operation_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
		const uint16_t temp16 = ctx.A + data + (ctx.P & C_MASK);
		const bool overflow   = ((~(ctx.A ^ data)) & (ctx.A ^ temp16) & 0x80) != 0;
		set_flag_condition<V_MASK>(ctx, overflow);
		set_flag_condition<C_MASK>(ctx, temp16 & 0x0100);
		ctx.A = temp16 & 0x00ff;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

