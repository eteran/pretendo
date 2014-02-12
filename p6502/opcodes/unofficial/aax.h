#ifndef AAX_20121206_H_
#define AAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_aax
// Desc: Store A & X
//------------------------------------------------------------------------------
struct opcode_aax {

	typedef operation_write memory_access;

	uint8_t operator()(Context &ctx, uint16_t address) const {
		(void)address;
		return ctx.A & ctx.X;
	}
};


#endif

