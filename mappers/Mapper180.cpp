
#include "Mapper180.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(180)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper180::Mapper180() {

	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff_ram(chr_ram_, 0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper180::name() const {
	return "UNROM/Crazy Climber";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper180::write_8(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cdef(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper180::write_9(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cdef(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper180::write_a(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cdef(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper180::write_b(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cdef(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper180::write_c(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cdef(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper180::write_d(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cdef(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper180::write_e(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cdef(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper180::write_f(uint_least16_t address, uint8_t value) {
	(void)address;
	set_prg_cdef(value & 0x07);
}
