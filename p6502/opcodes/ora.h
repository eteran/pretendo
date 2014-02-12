#ifndef ORA_20121206_H_
#define ORA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_ora
// Desc: Logical Inclusive OR
//------------------------------------------------------------------------------
struct opcode_ora {

	typedef operation_read memory_access;

	void operator()(Context &ctx, uint8_t data) const {
		ctx.A |= data;
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

