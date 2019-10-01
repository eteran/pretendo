
#include "Mapper034.h"
#include "Nes.h"
#include "Cart.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(34)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper34::Mapper34()  {

	// for now, this is how we tell the difference, iNES2.0 sub mappers
	// or UNIF would be preferable
	if(nes::cart.has_chr_rom()) {
		mode_ = NINA_001;
		set_prg_89ab(0);
		set_prg_cdef(-1);
		set_chr_0000_1fff(0);
	} else {
		mode_ = BNROM;
		set_prg_89ab(0);
		set_prg_cdef(-1);
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper34::name() const {
	switch(mode_) {
	case NINA_001:
		return "NINA-001";
	case BNROM:
		return "BNROM";
	}

	return "NINA-001/BNROM";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_7(uint16_t address, uint8_t value) {
	if(mode_ == NINA_001) {
		switch(address) {
		case 0x7ffd:
			set_prg_89abcdef(value);
			break;
		case 0x7ffe:
			set_chr_0000_0fff(value);
			break;
		case 0x7fff:
			set_chr_1000_1fff(value);
			break;
		default:
			Mapper::write_7(address, value);
		}
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_8(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_9(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_a(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_b(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_c(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_d(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_e(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_f(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper34::write_handler(uint16_t address, uint8_t value) {
	(void)address;
	if(mode_ == BNROM) {
		set_prg_89abcdef(value);
	}
}
