#ifndef STA_20121206_H_
#define STA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sta
// Desc: Store Accumulator
//------------------------------------------------------------------------------
struct opcode_sta {

	typedef operation_write memory_access;

	uint8_t operator()(Context &ctx, uint16_t address) const {
		(void)address;
		return ctx.A;
	}
};


#endif

