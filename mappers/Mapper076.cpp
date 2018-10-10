
#include "Mapper076.h"
#include "Cart.h"
#include "NES.h"
#include <cstring>
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(76)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper76::Mapper76() {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	set_chr_0000_1fff(0);
	
	command_ = 0;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper76::name() const {
	return "iNES 76";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_8(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_9(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_a(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_b(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_c(uint16_t address, uint8_t value) {
	write_handler(address, value);

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_d(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_e(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_f(uint16_t address, uint8_t value) {
	write_handler(address, value);
}


//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper76::write_handler(uint16_t address, uint8_t value) {

	switch(address & 0x8001) {
	case 0x8000:
		command_ = value;
		break;
	case 0x8001:
		switch(command_ & 0x07) {
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02: set_chr_0000_07ff(value & 0x3f); break;
		case 0x03: set_chr_0800_0fff(value & 0x3f); break;
		case 0x04: set_chr_1000_17ff(value & 0x3f); break;
		case 0x05: set_chr_1800_1fff(value & 0x3f); break;
		case 0x06: set_prg_89(value & 0x3f); break;
		case 0x07: set_prg_ab(value & 0x3f); break;
		}
	}
}
