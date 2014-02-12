#ifndef STX_20121206_H_
#define STX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_stx
// Desc: Store X Register
//------------------------------------------------------------------------------
struct opcode_stx {

	typedef operation_write memory_access;

	uint8_t operator()(Context &ctx, uint16_t address) const {
		(void)address;
		return ctx.X;
	}
};


#endif

