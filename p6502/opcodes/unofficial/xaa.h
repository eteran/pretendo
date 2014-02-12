#ifndef XAA_20121206_H_
#define XAA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_xaa
// Desc: Logical Inclusive OR
//------------------------------------------------------------------------------
struct opcode_xaa {

	typedef operation_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
	
		// who knows what the magic value should be...
	
		static const uint8_t magic = 0x00;
	
		ctx.A = (ctx.A | magic) & ctx.X & data;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

