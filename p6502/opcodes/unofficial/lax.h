#ifndef LAX_20121206_H_
#define LAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_lax
// Desc: Load A/X
//------------------------------------------------------------------------------
struct opcode_lax {

	typedef operation_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
		ctx.A = data;
		ctx.X = data;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

