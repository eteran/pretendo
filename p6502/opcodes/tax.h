#ifndef TAX_20121206_H_
#define TAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tax
// Desc: Transfer A to X
//------------------------------------------------------------------------------
struct opcode_tax {
	typedef operation_none memory_access;
	
	void operator()(Context &ctx) const {
		ctx.X = ctx.A;
		update_nz_flags(ctx, ctx.X);
	}
};

#endif

