#ifndef STA_20121206_H_
#define STA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sta
// Desc: Store Accumulator
//------------------------------------------------------------------------------
struct opcode_sta {

	typedef operation_write memory_access;

	static uint8_t execute(uint16_t address) {
		(void)address;
		return A;
	}
};


#endif

