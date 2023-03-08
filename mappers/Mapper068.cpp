
#include "Mapper068.h"
#include "Cart.h"
#include "Nes.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(68)

//------------------------------------------------------------------------------
// Name: Mapper68
//------------------------------------------------------------------------------
Mapper68::Mapper68() {
	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper68::name() const {
	return "Sunsoft Mapper #4";
}

//------------------------------------------------------------------------------
// Name: write_8
//------------------------------------------------------------------------------
void Mapper68::write_8(uint32_t address, uint8_t value) {
	(void)address;
	set_chr_0000_07ff(value);
}

//------------------------------------------------------------------------------
// Name: write_9
//------------------------------------------------------------------------------
void Mapper68::write_9(uint32_t address, uint8_t value) {
	(void)address;
	set_chr_0800_0fff(value);
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void Mapper68::write_a(uint32_t address, uint8_t value) {
	(void)address;
	set_chr_1000_17ff(value);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Mapper68::write_b(uint32_t address, uint8_t value) {
	(void)address;
	set_chr_1800_1fff(value);
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Mapper68::write_c(uint32_t address, uint8_t value) {
	(void)address;
	chr_rom_reg_[0] = ((value & 0x7f) | 0x80);
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Mapper68::write_d(uint32_t address, uint8_t value) {
	(void)address;
	chr_rom_reg_[1] = ((value & 0x7f) | 0x80);
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Mapper68::write_e(uint32_t address, uint8_t value) {
	(void)address;

	reg_e000_ = value;

	switch (value & 0x01) {
	case 0x00:
		set_mirroring(mirror_horizontal);
		break;
	case 0x01:
		set_mirroring(mirror_vertical);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper68::write_f(uint32_t address, uint8_t value) {
	(void)address;
	set_prg_89ab(value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper68::read_vram(uint32_t address) {

	if (reg_e000_ & 0x10) {
		const uint8_t *const nt_chr_c000_ = nes::cart.chr() + ((chr_rom_reg_[0] * 0x400) & nes::cart.chr_mask());
		const uint8_t *const nt_chr_d000_ = nes::cart.chr() + ((chr_rom_reg_[1] * 0x400) & nes::cart.chr_mask());

		switch ((address >> 10) & 0x0f) {
		case 0x08:
		case 0x0c:
			// $2000
			return nt_chr_c000_[address & 0x3ff];
		case 0x09:
		case 0x0d:
			// $2400
			if (reg_e000_ & 0x01) {
				return nt_chr_c000_[address & 0x3ff];
			} else {
				return nt_chr_d000_[address & 0x3ff];
			}
		case 0x0a:
		case 0x0e:
			// $2800
			if (reg_e000_ & 0x01) {
				return nt_chr_d000_[address & 0x3ff];
			} else {
				return nt_chr_c000_[address & 0x3ff];
			}
		case 0x0b:
		case 0x0f:
			// $2c00
			return nt_chr_d000_[address & 0x3ff];
		default:
			break;
		}
	}

	return Mapper::read_vram(address);
}
