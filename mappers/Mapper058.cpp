
#include "Mapper058.h"
#include "Cart.h"
#include "Nes.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(58)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper58::Mapper58() {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	if (nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper58::name() const {
	return "iNES 58";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_8(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_9(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_a(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_b(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_c(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_d(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_e(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_f(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper58::write_hander(uint_least16_t address, uint8_t value) {

	(void)value;

	if (address & 0x80) {
		set_mirroring(mirror_horizontal);
	} else {
		set_mirroring(mirror_vertical);
	}

	if (address & 0x40) {
		set_prg_89ab(address & 0x07);
		set_prg_cdef(address & 0x07);
	} else {
		set_prg_89abcdef(address & 0x07);
	}

	set_chr_0000_1fff((address >> 3) & 0x07);
}
