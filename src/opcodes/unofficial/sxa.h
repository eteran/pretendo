#ifndef SXA_20121206_H_
#define SXA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sxa
// Desc: AND X register with the high byte of the target address of the
//       argument + 1. Store the result in memory.
//------------------------------------------------------------------------------
struct opcode_sxa {

	using memory_access = operation_write;

	static uint8_t execute(uint_least16_t &address) {
		// NOTE(eteran): we don't need an explicit +1, because
		// it is automatically caused by the data fetch
		const uint8_t value = (address >> 8) & X;
		address             = (address & 0x00ff) | (value << 8);
		return value;
	}
};

#endif
