
#include "Mapper033.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(33)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper33::Mapper33() {
	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper33::name() const {
	return "Taito TC019";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper33::write_8(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper33::write_9(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper33::write_a(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper33::write_b(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper33::write_handler(uint16_t address, uint8_t value) {

	switch (address & 0xa003) {
	case 0x8000:
		if (!(value & 0x40)) {
			set_mirroring(mirror_vertical);
		} else {
			set_mirroring(mirror_horizontal);
		}
		set_prg_89(value & 0x3f);
		break;
	case 0x8001:
		set_prg_ab(value & 0x3f);
		break;
	case 0x8002:
		set_chr_0000_07ff(value);
		break;
	case 0x8003:
		set_chr_0800_0fff(value);
		break;
	case 0xa000:
		set_chr_1000_13ff(value);
		break;
	case 0xa001:
		set_chr_1400_17ff(value);
		break;
	case 0xa002:
		set_chr_1800_1bff(value);
		break;
	case 0xa003:
		set_chr_1c00_1fff(value);
		break;
	}
}
