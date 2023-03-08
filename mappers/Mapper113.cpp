
#include "Mapper113.h"
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(113)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper113::Mapper113() {
	set_prg_89abcdef(0);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper113::name() const {
	return "iNES 113";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper113::write_4(uint32_t address, uint8_t value) {
	if ((address & 0x4100) == 0x4100) {
		write_handler(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper113::write_5(uint32_t address, uint8_t value) {
	if ((address & 0x4100) == 0x4100) {
		write_handler(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper113::write_handler(uint32_t address, uint8_t value) {
	assert((address & 0x4100) == 0x4100);

	(void)address;

	set_prg_89abcdef((value >> 3) & 0x07);
	set_chr_0000_1fff((value & 0x07) | ((value & 0x40) >> 3));

	if (value & 0x80) {
		set_mirroring(mirror_vertical);
	} else {
		set_mirroring(mirror_horizontal);
	}
}
