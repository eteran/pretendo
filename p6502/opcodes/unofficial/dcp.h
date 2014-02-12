#ifndef DCP_20121206_H_
#define DCP_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_dcp
// Desc: Decrement Memory (no borrow)
//------------------------------------------------------------------------------
struct opcode_dcp  : compare {

	typedef operation_modify memory_access;
	
	void operator()(Context &ctx, uint8_t &data) const {
		--data;
		update_nz_flags(ctx, data);
		do_compare(ctx, ctx.A, data);
	}
};

#endif

