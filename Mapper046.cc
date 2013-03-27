
#include "Mapper046.h"
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(46);

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper46::Mapper46() : prg_(0), chr_(0) {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	assert(nes::cart.has_chr_rom());
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper46::name() const {
	return "Rumblestation 15-in-1";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write67(uint16_t address, uint8_t value) {
	(void)address;
	prg_ = (prg_ & 0x01) | ((value & 0x0f) << 1);
	chr_ = (chr_ & 0x07) | ((value & 0xf0) >> 1);
	set_prg_89abcdef(prg_);
	set_chr_0000_1fff(chr_);
}
//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write89ABCDEF(uint16_t address, uint8_t value) {
	(void)address;
	prg_ = (prg_ & 0xfe) | (value & 0x01);
	chr_ = (chr_ & 0xf8) | ((value & 0x70) >> 4);
	set_prg_89abcdef(prg_);
	set_chr_0000_1fff(chr_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_6(uint16_t address, uint8_t value) {
	write67(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_7(uint16_t address, uint8_t value) {
	write67(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_8(uint16_t address, uint8_t value) {
	write89ABCDEF(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_9(uint16_t address, uint8_t value) {
	write89ABCDEF(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_a(uint16_t address, uint8_t value) {
	write89ABCDEF(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_b(uint16_t address, uint8_t value) {
	write89ABCDEF(address, value);;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_c(uint16_t address, uint8_t value) {
	write89ABCDEF(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_d(uint16_t address, uint8_t value) {
	write89ABCDEF(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_e(uint16_t address, uint8_t value) {
	write89ABCDEF(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper46::write_f(uint16_t address, uint8_t value) {
	write89ABCDEF(address, value);
}
