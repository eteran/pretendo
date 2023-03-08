
#include "Mapper015.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(15)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper15::Mapper15() {
	set_prg_89abcdef(0);
	set_chr_0000_1fff_ram(chr_ram_, 0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper15::name() const {
	return "100-in-1";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_8(uint32_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_9(uint32_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_a(uint32_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_b(uint32_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_c(uint32_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_d(uint32_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_e(uint32_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_f(uint32_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper15::write_handler(uint32_t address, uint8_t value) {

	uint8_t page89;
	uint8_t pageAB;
	uint8_t pageCD;
	uint8_t pageEF;

	switch (address & 0x03) {
	case 0x00:
		page89 = (((value & 0x3f) | 0) << 1);
		pageAB = page89 + 1;

		pageCD = (((value & 0x3f) | 1) << 1);
		pageEF = pageCD + 1;
		break;
	case 0x01:
		page89 = (((value & 0x3f) | 0) << 1);
		pageAB = page89 + 1;

		pageCD = -2;
		pageEF = -1;
		break;
	case 0x02:
		page89 =
			pageAB =
				pageCD =
					pageEF =
						(((value & 0x3f) + 0) << 1) + ((value & 0x80) >> 7);
		break;
	case 0x03:
	default:
		page89 =
			pageCD =
				((value & 0x3f) << 1);

		pageAB =
			pageEF =
				pageCD + 1;

		break;
	}

	set_prg_89(page89);
	set_prg_ab(pageAB);
	set_prg_cd(pageCD);
	set_prg_ef(pageEF);

	if (value & 0x40) {
		set_mirroring(mirror_horizontal);
	} else {
		set_mirroring(mirror_vertical);
	}
}
