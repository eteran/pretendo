
#include "Mapper032.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(32);

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper32::Mapper32() : prg_8000_(0), prg_a000_(0), prg_mode_(0) {

	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper32::name() const {
	return "Irem G-101";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper32::sync_prg() {
	if(prg_mode_) {
		set_prg_89(0);
		set_prg_ab(prg_a000_);
		set_prg_cd(prg_8000_);
		set_prg_ef(-1);
	} else {
		set_prg_89(prg_8000_);
		set_prg_ab(prg_a000_);
		set_prg_cd(-2);
		set_prg_ef(-1);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper32::write_8(uint16_t address, uint8_t value) {
	(void)address;
	prg_8000_ = value;
	sync_prg();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper32::write_9(uint16_t address, uint8_t value) {
	(void)address;
	prg_mode_ = (value & 0x02) >> 1;

	// do we do a PRG swap on mode change?
	sync_prg();

#if 1
	// this code should be off for major league
	if(value & 0x01) {
		nes::ppu.set_mirroring(PPU::mirror_horizontal);
	} else {
		nes::ppu.set_mirroring(PPU::mirror_vertical);
	}
#endif
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper32::write_a(uint16_t address, uint8_t value) {
	(void)address;
	prg_a000_ = value;
	sync_prg();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper32::write_b(uint16_t address, uint8_t value) {
	switch(address & 0xf007) {
	case 0xb000:
		set_chr_0000_03ff(value);
		break;
	case 0xb001:
		set_chr_0400_07ff(value);
		break;
	case 0xb002:
		set_chr_0800_0bff(value);
		break;
	case 0xb003:
		set_chr_0c00_0fff(value);
		break;
	case 0xb004:
		set_chr_1000_13ff(value);
		break;
	case 0xb005:
		set_chr_1400_17ff(value);
		break;
	case 0xb006:
		set_chr_1800_1bff(value);
		break;
	case 0xb007:
		set_chr_1c00_1fff(value);
		break;
	}
}

