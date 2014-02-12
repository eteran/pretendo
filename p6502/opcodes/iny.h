#ifndef INY_20121206_H_
#define INY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_iny
// Desc: Increment Y Register
//------------------------------------------------------------------------------
struct opcode_iny {

	typedef operation_none memory_access;

	void operator()(Context &ctx) const {
		update_nz_flags(ctx, ++ctx.Y);
	}
};


#endif

