#ifndef AXS_20121206_H_
#define AXS_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_axs
// Desc: Transfer A to X
//------------------------------------------------------------------------------
struct opcode_axs {
	typedef operation_read memory_access;
	
	void operator()(Context &ctx, uint8_t data) const {
		const uint16_t new_x = (ctx.X & ctx.A) - data;
		const bool carry     = (new_x < 0x100);
		ctx.X = new_x & 0xff;
		update_nz_flags(ctx, ctx.X);
		set_flag_condition<C_MASK>(ctx, carry);
	}
};

#endif

