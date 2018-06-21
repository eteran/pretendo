#ifndef SXA_20121206_H_
#define SXA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sxa
// Desc: AND X register with the high byte of the target address of the
//       argument + 1. Store the result in memory.
//------------------------------------------------------------------------------
struct opcode_sxa {

	using memory_access = operation_write;

	static uint8_t execute(uint16_t &address) {
	
		const uint8_t high_byte = ((address >> 8) & 0xff) + 1;
		const uint8_t value     = (X & high_byte);
		address = (address & 0x00ff) | (value << 8);
		return X & high_byte;
	}
};


#endif

