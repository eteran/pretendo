
#include "Mapper075.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(75)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper75::Mapper75() {
	set_prg_89abcdef(0);
	set_prg_ef(-1);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper75::name() const {
	return "VRC1";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper75::write_8(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_89(value & 0x0f);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper75::write_9(uint_least16_t address, uint8_t value) {
	(void)address;
	if (value & 0x01) {
		set_mirroring(mirror_horizontal);
	} else {
		set_mirroring(mirror_vertical);
	}

	chr_reg_[0] = (chr_reg_[0] & 0x0f) | ((value << 3) & 0x10);
	chr_reg_[1] = (chr_reg_[1] & 0x0f) | ((value << 2) & 0x10);

	set_chr_0000_0fff(chr_reg_[0]);
	set_chr_1000_1fff(chr_reg_[1]);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper75::write_a(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_ab(value & 0x0f);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper75::write_c(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cd(value & 0x0f);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper75::write_e(uint_least16_t address, uint8_t value) {
	(void)address;
	chr_reg_[0] = (chr_reg_[0] & 0xf0) | (value & 0x0f);
	set_chr_0000_0fff(chr_reg_[0]);
	set_chr_1000_1fff(chr_reg_[1]);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper75::write_f(uint_least16_t address, uint8_t value) {
	(void)address;
	chr_reg_[1] = (chr_reg_[1] & 0xf0) | (value & 0x0f);
	set_chr_0000_0fff(chr_reg_[0]);
	set_chr_1000_1fff(chr_reg_[1]);
}
