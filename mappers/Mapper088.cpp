
#include "Mapper088.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(88)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper88::Mapper88() : command_(0) {

	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper88::name() const {
	return "iNES 88";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper88::write_8(uint16_t address, uint8_t value) {

	switch(address & 0x000f) {
	case 0x8000:
		command_ = value & 0x07;
		break;

	case 0x8001:
		switch(command_) {
		case 0x00: set_chr_0000_07ff((value & 0x3e)); break;
		case 0x01: set_chr_0800_0fff((value & 0x3e)); break;
		case 0x02: set_chr_1000_13ff((value & 0x3f) | 0x40); break;
		case 0x03: set_chr_1400_17ff((value & 0x3f) | 0x40); break;
		case 0x04: set_chr_1800_1bff((value & 0x3f) | 0x40); break;
		case 0x05: set_chr_1c00_1fff((value & 0x3f) | 0x40); break;
		case 0x06: set_prg_89(value & 0x0f); break;
		case 0x07: set_prg_ab(value & 0x0f); break;
		default:
			break;
		}
	}

}

