#ifndef DEY_20121206_H_
#define DEY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_dey
// Desc: Decrement Y Register
//------------------------------------------------------------------------------
struct opcode_dey {

	typedef operation_none memory_access;

	void operator()(Context &ctx) const {
		update_nz_flags(ctx, --ctx.Y);
	}
};


#endif

