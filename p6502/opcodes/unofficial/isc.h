#ifndef ISC_20121206_H_
#define ISC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_isc
// Desc: Increment Memory & CMP
//------------------------------------------------------------------------------
struct opcode_isc : compare {

	typedef operation_modify memory_access;
	
	void operator()(Context &ctx, uint8_t &data) const {
		++data;
		const uint16_t temp16 = ctx.A - data - ((ctx.P & C_MASK) ^ C_MASK);
		const bool carry	= (temp16 < 0x100);
		const bool overflow	= ((ctx.A ^ data) & (ctx.A ^ temp16) & 0x80) != 0;
		set_flag_condition<V_MASK>(ctx, overflow);
		set_flag_condition<C_MASK>(ctx, carry);
		ctx.A = (temp16 & 0x00ff);
		update_nz_flags(ctx, ctx.A);
	}
};

#endif

