
#include "Mapper058.h"
#include "PPU.h"
#include "NES.h"
#include "Cart.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(58)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper58::Mapper58() {

	set_prg_89ab(0);
	set_prg_cdef(-1);

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
std::string Mapper58::name() const {
	return "iNES 58";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_8(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_9(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_a(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_b(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_c(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_d(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_e(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_f(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_hander(uint16_t address, uint8_t value) {

	(void)value;

	if(address & 0x80) {
		set_mirroring(nes::ppu::mirror_horizontal);
	} else {
		set_mirroring(nes::ppu::mirror_vertical);
	}

	if(address & 0x40) {
		set_prg_89ab(address & 0x07);
		set_prg_cdef(address & 0x07);
	} else {
		set_prg_89abcdef(address & 0x07);
	}


	set_chr_0000_1fff((address >> 3) & 0x07);
}
