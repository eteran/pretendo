
#include "Mapper009.h"
#include "Cart.h"
#include "Nes.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(9)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper9::Mapper9() {
	set_prg_89(0);
	set_prg_ab(-3);
	set_prg_cd(-2);
	set_prg_ef(-1);

	if (nes::cart.has_chr_rom()) {
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
void Mapper9::write_a(uint32_t address, uint8_t value) {
	(void)address;
	set_prg_89(value & 0x0f);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Mapper9::write_b(uint32_t address, uint8_t value) {
	(void)address;
	latch0_lo_ = value;

	if (!latch0_) {
		set_chr_0000_0fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Mapper9::write_c(uint32_t address, uint8_t value) {
	(void)address;
	latch0_hi_ = value;

	if (latch0_) {
		set_chr_0000_0fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Mapper9::write_d(uint32_t address, uint8_t value) {
	(void)address;
	latch1_lo_ = value;

	if (!latch1_) {
		set_chr_1000_1fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Mapper9::write_e(uint32_t address, uint8_t value) {
	(void)address;
	latch1_hi_ = value;

	if (latch1_) {
		set_chr_1000_1fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void Mapper9::write_f(uint32_t address, uint8_t value) {
	(void)address;
	if (value & 0x01) {
		set_mirroring(mirror_horizontal);
	} else {
		set_mirroring(mirror_vertical);
	}
}

//------------------------------------------------------------------------------
// Name: read_vram
//------------------------------------------------------------------------------
uint8_t Mapper9::read_vram(uint32_t address) {

	const uint8_t ret = Mapper::read_vram(address);

	switch (address) {
	case 0x0fd8:
		set_chr_0000_0fff(latch0_lo_);
		latch0_ = false;
		break;
	case 0x0fe8:
		set_chr_0000_0fff(latch0_hi_);
		latch0_ = true;
		break;
	case 0x1fd8:
	case 0x1fd9:
	case 0x1fda:
	case 0x1fdb:
	case 0x1fdc:
	case 0x1fdd:
	case 0x1fde:
	case 0x1fdf:
		set_chr_1000_1fff(latch1_lo_);
		latch1_ = false;
		break;
	case 0x1fe8:
	case 0x1fe9:
	case 0x1fea:
	case 0x1feb:
	case 0x1fec:
	case 0x1fed:
	case 0x1fee:
	case 0x1fef:
		set_chr_1000_1fff(latch1_hi_);
		latch1_ = true;
		break;
	}

	return ret;
}
