
#include "Mapper061.h"
#include "PPU.h"
#include "NES.h"
#include "Cart.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(61)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper61::Mapper61() {

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
std::string Mapper61::name() const {
	return "iNES 61";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_8(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_9(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_a(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_b(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_c(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_d(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_e(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_f(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_hander(uint16_t address, uint8_t value) {

	(void)value;

	if(address & 0x80) {
		nes::ppu::set_mirroring(nes::ppu::mirror_horizontal);
	} else {
		nes::ppu::set_mirroring(nes::ppu::mirror_vertical);
	}

	const uint8_t prg = ((address << 1) & 0x1e) | ((address >> 5) & 0x01);

	if(address & 0x10) {
		set_prg_89ab(prg);
		set_prg_cdef(prg);
	} else {
		set_prg_89abcdef(prg);
	}
}
