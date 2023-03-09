
#include "Mapper061.h"
#include "Cart.h"
#include "Nes.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(61)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper61::Mapper61() {

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
std::string Mapper61::name() const {
	return "iNES 61";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_8(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_9(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_a(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_b(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_c(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_d(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_e(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_f(uint_least16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper61::write_hander(uint_least16_t address, uint8_t value) {

	(void)value;

	if (address & 0x80) {
		set_mirroring(mirror_horizontal);
	} else {
		set_mirroring(mirror_vertical);
	}

	const uint8_t prg = ((address << 1) & 0x1e) | ((address >> 5) & 0x01);

	if (address & 0x10) {
		set_prg_89ab(prg);
		set_prg_cdef(prg);
	} else {
		set_prg_89abcdef(prg);
	}
}
