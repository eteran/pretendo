#ifndef STA_20121206_H_
#define STA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sta
// Desc: Store Accumulator
//------------------------------------------------------------------------------
struct opcode_sta {

	using memory_access = operation_write;

	static uint8_t execute(uint16_t address) {
		(void)address;
		return A;
	}
};

#endif
