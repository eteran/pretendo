#ifndef SYA_20121206_H_
#define SYA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sya
// Desc: AND Y register with the high byte of the target address of the
//       argument + 1. Store the result in memory.
//------------------------------------------------------------------------------
struct opcode_sya {

	typedef operation_write memory_access;

	uint8_t operator()(uint16_t &address) const {
		
		const uint8_t high_byte = ((address >> 8) & 0xff) + 1;
		const uint8_t value     = (Y & high_byte);
		address = (address & 0x00ff) | (value << 8);
		return Y & high_byte;
	}
};


#endif

