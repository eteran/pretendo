#ifndef TSX_20121206_H_
#define TSX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tsx
// Desc: Transfer S to X
//------------------------------------------------------------------------------
struct opcode_tsx {
	typedef operation_none memory_access;
	
	void operator()(Context &ctx) const {
		ctx.X = ctx.S;
		update_nz_flags(ctx, ctx.X);
	}
};

#endif

