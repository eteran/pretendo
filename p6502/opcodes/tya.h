#ifndef TYA_20121206_H_
#define TYA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tya
// Desc: Transfer Y to A
//------------------------------------------------------------------------------
struct opcode_tya {
	typedef operation_none memory_access;
	
	void operator()(Context &ctx) const {
		ctx.A = ctx.Y;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

