
#include "Mapper009.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(9);

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper9::Mapper9() : latch1_(true), latch2_(true), latch1_lo_(0), latch1_hi_(0), latch2_lo_(0), latch2_hi_(0) {

	memset(chr_ram_, 0, sizeof(chr_ram_));

	set_prg_89(0);
	set_prg_ab(-3);
	set_prg_cd(-2);
	set_prg_ef(-1);

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper9::name() const {
	return "PxROM (Nintendo MMC2)";
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void Mapper9::write_a(uint16_t address, uint8_t value) {
	(void)address;
	set_prg_89(value & 0x0f);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Mapper9::write_b(uint16_t address, uint8_t value) {
	(void)address;
	latch1_lo_ = value;

	if(!latch1_) {
		set_chr_0000_0fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Mapper9::write_c(uint16_t address, uint8_t value) {
	(void)address;
	latch1_hi_ = value;

	if(latch1_) {
		set_chr_0000_0fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Mapper9::write_d(uint16_t address, uint8_t value) {
	(void)address;
	latch2_lo_ = value;

	if(!latch2_) {
		set_chr_1000_1fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Mapper9::write_e(uint16_t address, uint8_t value) {
	(void)address;
	latch2_hi_ = value;

	if(latch2_) {
		set_chr_1000_1fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void Mapper9::write_f(uint16_t address, uint8_t value) {
	(void)address;
	if(value & 0x01) {
		nes::ppu.set_mirroring(PPU::mirror_horizontal);
	} else {
		nes::ppu.set_mirroring(PPU::mirror_vertical);
	}
}

//------------------------------------------------------------------------------
// Name: read_vram
//------------------------------------------------------------------------------
uint8_t Mapper9::read_vram(uint16_t address) {

	const uint8_t ret = Mapper::read_vram(address);

	switch(address & 0xfff0) {
	case 0x0fd0:
		set_chr_0000_0fff(latch1_lo_);
		latch1_ = false;
		break;
	case 0x1fd0:
		set_chr_1000_1fff(latch2_lo_);
		latch2_ = false;
		break;
	case 0x0fe0:
		set_chr_0000_0fff(latch1_hi_);
		latch1_ = true;
		break;
	case 0x1fe0:
		set_chr_1000_1fff(latch2_hi_);
		latch2_ = true;
		break;
	}

	return ret;
}
