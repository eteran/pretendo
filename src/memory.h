
#ifndef MEMORY_20070622_H_
#define MEMORY_20070622_H_

//------------------------------------------------------------------------------
// Name: read_byte
// Desc: reads a value from memory including mapper and I/O
//------------------------------------------------------------------------------
inline uint8_t read_byte(uint_least16_t address) {
	return nes::bus::read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_byte_zp
// Desc: reads a value from memory including mapper and I/O
//------------------------------------------------------------------------------
inline uint8_t read_byte_zp(uint8_t address) {
	return nes::bus::read_memory(address);
}

//------------------------------------------------------------------------------
// Name: write_byte
// Desc: writes a value to memory including mapper and I/O
//------------------------------------------------------------------------------
inline void write_byte(uint_least16_t address, uint8_t value) {
	nes::bus::write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_byte_zp
// Desc: writes a value to memory including mapper and I/O
//------------------------------------------------------------------------------
inline void write_byte_zp(uint8_t address, uint8_t value) {
	nes::bus::write_memory(address, value);
}

#endif
