#ifndef AAC_20121206_H_
#define AAC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_aac
// Desc: Logical AND then copies N to C
//------------------------------------------------------------------------------
struct opcode_aac {

	typedef operation_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
		ctx.A &= data;
		update_nz_flags(ctx, ctx.A);
		ctx.P = (ctx.P & ~C_MASK) | ((ctx.P & N_MASK) >> 7);
	}
};

#endif

