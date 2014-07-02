
#include "Mapper057.h"
#include "Cart.h"
#include "PPU.h"
#include <cstring>
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(57);

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper57::Mapper57() : reg0_(0), reg1_(0) {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	assert(nes::cart.has_chr_rom());
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper57::name() const {
	return "Mapper 57 (Pirate)";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_8(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_9(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_a(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_b(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_c(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_d(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_e(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_f(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper57::write_hander(uint16_t address, uint8_t value) {

	switch(address & 0x8800) {
	case 0x8000:
		reg0_ = value;
		break;
	case 0x8800:
		reg1_ = value;
		break;
	}

	// set mirroring
	if(reg1_ & 0x08) {
		nes::ppu::set_mirroring(nes::ppu::mirror_horizontal);
	} else {
		nes::ppu::set_mirroring(nes::ppu::mirror_vertical);
	}

	if(reg1_ & 0x10) {
		set_prg_89abcdef((reg1_ >> 5) & 0x07);
	} else {
		set_prg_89ab((reg1_ >> 5) & 0x07);
		set_prg_cdef((reg1_ >> 5) & 0x07);
	}


	set_chr_0000_1fff((reg0_ & 0x07) | (reg1_ & 0x07) | ((reg0_ >> 3) & 0x08));
}
