#ifndef PLA_20121206_H_
#define PLA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_pla
// Desc: Pull Accumulator
//------------------------------------------------------------------------------
struct opcode_pla {

	typedef operation_stack_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
		ctx.A = data;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

