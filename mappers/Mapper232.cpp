
#include "Mapper232.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(232)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper232::Mapper232() {

	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff_ram(chr_ram_, 0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper232::name() const {
	return "Camerica/Codemasters";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper232::write_8(uint_least16_t address, uint8_t value) {
	(void)address;
	(void)value;
#if 0
	// firehawk only
	if(value & 0x10) {
		set_mirroring(mirror_single_high);
	} else {
		set_mirroring(mirror_single_low);
	}
#endif

	// TODO: Quattro carts had some sort of 64KB ROM bank select here too
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper232::write_9(uint_least16_t address, uint8_t value) {
	write_8(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper232::write_c(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_89ab(value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper232::write_d(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_89ab(value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper232::write_e(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_89ab(value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper232::write_f(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_89ab(value);
}
