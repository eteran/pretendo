
#include "Mapper231.h"
#include "PPU.h"
#include "NES.h"
#include "Cart.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(231)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper231::Mapper231() {

	set_prg_89ab(0);
	set_prg_cdef(0);
	
	memset(chr_ram_, 0, sizeof(chr_ram_));

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper231::name() const {
	return "20-in-1";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_8(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_9(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_a(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_b(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_c(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_d(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_e(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_f(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper231::write_handler(uint16_t address, uint8_t value) {
	(void)value;
	if(address & 0x80) {
		nes::ppu::set_mirroring(nes::ppu::mirror_horizontal);
	} else {
		nes::ppu::set_mirroring(nes::ppu::mirror_vertical);
	}

	prg_ = (prg_ & 0xfe) | ((address & 0x20) >> 5);
	prg_ = (prg_ & 0x01) | ((address & 0x1e));
	set_prg_89ab(prg_);

}
