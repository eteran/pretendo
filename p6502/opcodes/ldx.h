#ifndef LDX_20121206_H_
#define LDX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_ldx
// Desc: Load X Register
//------------------------------------------------------------------------------
struct opcode_ldx {

	typedef operation_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
		ctx.X = data;
		update_nz_flags(ctx, ctx.X);
	}
};

#endif

