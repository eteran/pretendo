
#ifndef AXA_20121206_H_
#define AXA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_axa
// Desc: AND X register with accumulator then AND result with 7 and store in memory.
// Alias: SHA, AHX
//------------------------------------------------------------------------------
struct opcode_axa {

	using memory_access = operation_write;

	static uint8_t execute(uint_least16_t &address) {
		// NOTE(eteran): we don't need an explicit +1, because
		// it is automatically caused by the data fetch
		const uint8_t value = (address >> 8) & X & A;
		address             = (address & 0x00ff) | (value << 8);
		return value;
	}
};

#endif
