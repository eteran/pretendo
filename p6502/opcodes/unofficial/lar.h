#ifndef LAR_20121206_H_
#define LAR_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_lar
// Desc: AND memory with S, transfer result to A, X and S.
//------------------------------------------------------------------------------
struct opcode_lar {

	typedef operation_read memory_access;
	
	void operator()(Context &ctx, uint8_t data) const {

		data &= ctx.S;
		ctx.A = data;
		ctx.X = data;
		ctx.S = data;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

