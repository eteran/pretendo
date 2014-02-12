#ifndef TAY_20121206_H_
#define TAY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tay
// Desc: Transfer A to Y
//------------------------------------------------------------------------------
struct opcode_tay {
	typedef operation_none memory_access;
	
	void operator()(Context &ctx) const {
		ctx.Y = ctx.A;
		update_nz_flags(ctx, ctx.Y);
	}
};

#endif

