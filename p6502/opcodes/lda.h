#ifndef LDA_20121206_H_
#define LDA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_lda
// Desc: Load Accumulator
//------------------------------------------------------------------------------
struct opcode_lda {

	typedef operation_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
		ctx.A = data;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

