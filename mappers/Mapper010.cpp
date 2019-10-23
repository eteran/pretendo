
#include "Mapper010.h"
#include "Cart.h"
#include "Nes.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(10)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper10::Mapper10() {
	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper10::name() const {
	return "FxROM (Nintendo MMC4)";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper10::write_a(uint16_t address, uint8_t value) {
	(void)address;
	set_prg_89ab(value & 0x0f);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Mapper10::write_b(uint16_t address, uint8_t value) {
	(void)address;
	latch0_lo_ = value;

	if(!latch0_) {
		set_chr_0000_0fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Mapper10::write_c(uint16_t address, uint8_t value) {
	(void)address;
	latch0_hi_ = value;

	if(latch0_) {
		set_chr_0000_0fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Mapper10::write_d(uint16_t address, uint8_t value) {
	(void)address;
	latch1_lo_ = value;

	if(!latch1_) {
		set_chr_1000_1fff(value & 0x1f);
	}
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Mapper10::write_e(uint16_t address, uint8_t value) {
	(void)address;
	latch1_hi_ = value;

	if(latch1_) {
		set_chr_1000_1fff(value & 0x1f);
	}
}
//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper10::write_f(uint16_t address, uint8_t value) {
	(void)address;
	if(value & 0x01) {
		set_mirroring(mirror_horizontal);
	} else {
		set_mirroring(mirror_vertical);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper10::read_vram(uint16_t address) {

	const uint8_t ret = Mapper::read_vram(address);

	switch(address & 0xfff8) {
	case 0x0fd8:
		set_chr_0000_0fff(latch0_lo_);
		latch0_ = false;
		break;
	case 0x0fe8:
		set_chr_0000_0fff(latch0_hi_);
		latch0_ = true;
		break;
	case 0x1fd8:
		set_chr_1000_1fff(latch1_lo_);
		latch1_ = false;
		break;
	case 0x1fe8:
		set_chr_1000_1fff(latch1_hi_);
		latch1_ = true;
		break;
	}

	return ret;
}
