
#ifndef MEMORY_20070622_H_
#define MEMORY_20070622_H_

//------------------------------------------------------------------------------
// Name: read_byte
// Desc: reads a value from memory including mapper and I/O
//------------------------------------------------------------------------------
inline uint8_t read_byte(const Context &ctx, uint16_t address) {
	return (*ctx.read_handler)(address);
}

//------------------------------------------------------------------------------
// Name: read_byte_zp
// Desc: reads a value from memory including mapper and I/O
//------------------------------------------------------------------------------
inline uint8_t read_byte_zp(const Context &ctx, uint8_t address) {
	return (*ctx.read_handler)(address);
}

//------------------------------------------------------------------------------
// Name: write_byte
// Desc: writes a value to memory including mapper and I/O
//------------------------------------------------------------------------------
inline void write_byte(const Context &ctx, uint16_t address, uint8_t value) {
	(*ctx.write_handler)(address, value);
}

//------------------------------------------------------------------------------
// Name: write_byte_zp
// Desc: writes a value to memory including mapper and I/O
//------------------------------------------------------------------------------
inline void write_byte_zp(const Context &ctx, uint8_t address, uint8_t value) {
	(*ctx.write_handler)(address, value);
}

#endif
