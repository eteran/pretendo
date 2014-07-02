
#include "Mapper228.h"
#include "PPU.h"
#include "Cart.h"
#include <cstdio>
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(228);

namespace {
	uint8_t *prg_chips[4];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper228::Mapper228() {

	ram_[0] = 0;
	ram_[1] = 0;
	ram_[2] = 0;
	ram_[3] = 0;

	prg_chips[0] = nes::cart.prg() + 0x000000;
	prg_chips[1] = nes::cart.prg() + 0x080000;
	prg_chips[2] = nullptr; // TODO: implement open bus effect here...
	prg_chips[3] = nes::cart.prg() + 0x100000;

	write_hander(0x8000, 0x00);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper228::name() const {
	return "Action 52";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_4(uint16_t address) {
	if(address >= 0x4020) {
		return ram_[address & 0x03];
	}
	return Mapper::read_4(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_5(uint16_t address) {
	return ram_[address & 0x03];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_4(uint16_t address, uint8_t value) {

	if(address >= 0x4020) {
		ram_[address & 0x03] = value & 0x03;
	} else {
		Mapper::write_4(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_5(uint16_t address, uint8_t value) {
	ram_[address & 0x03] = value & 0x03;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_8(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_9(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_a(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_b(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_c(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_d(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_e(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_f(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_hander(uint16_t address, uint8_t value) {

	if(address & 0x2000) {
		nes::ppu::set_mirroring(nes::ppu::mirror_horizontal);
	} else {
		nes::ppu::set_mirroring(nes::ppu::mirror_vertical);
	}

	uint8_t prg_chip_select        = (address >> 11) & 0x03;
	const uint8_t  prg_page_select = (address >> 6) & 0x1f;
	const uint8_t  prg_mode        = ((address & 0x20) != 0);
	const uint8_t  chr_select      = ((address & 0x0f) << 2) | (value & 0x03);
	const uint32_t prg_mask        = nes::cart.prg_mask();

	set_chr_0000_1fff(chr_select);

	assert(prg_chip_select != 2);

	printf("prg_chip_select = %d\n", prg_chip_select);

	// Cheetahmen II seems to only have one chip
	if(prg_mask < 0x7ffff) {
		prg_chip_select = 0;
	}

	if(prg_mode == 0) {
		set_prg_89abcdef_ptr(prg_chips[prg_chip_select] + (((prg_page_select >> 1) * 0x8000) & 0x7ffff));
	} else if(prg_mode == 1) {
		set_prg_89ab_ptr(prg_chips[prg_chip_select] + ((prg_page_select * 0x4000) & 0x7ffff));
		set_prg_cdef_ptr(prg_chips[prg_chip_select] + ((prg_page_select * 0x4000) & 0x7ffff));
	}
}
