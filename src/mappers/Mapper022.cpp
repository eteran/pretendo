
#include "Mapper022.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(22)

//------------------------------------------------------------------------------
// Name: Mapper22
//------------------------------------------------------------------------------
Mapper22::Mapper22()
	: VRC2(1) {
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper22::name() const {
	return "VRC2a";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper22::read_6(uint_least16_t address) {
	switch (address & 0x0003) {
	case 0x0000:
		return VRC2::read_6(0x6000);
	case 0x0001:
		return VRC2::read_6(0x6002);
	case 0x0002:
		return VRC2::read_6(0x6001);
	case 0x0003:
		return VRC2::read_6(0x6003);
	}

	return 0xff;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_6(uint_least16_t address, uint8_t value) {
	switch (address & 0x0003) {
	case 0x0000:
		VRC2::write_6(0x6000, value);
		break;
	case 0x0001:
		VRC2::write_6(0x6002, value);
		break;
	case 0x0002:
		VRC2::write_6(0x6001, value);
		break;
	case 0x0003:
		VRC2::write_6(0x6003, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_8(uint_least16_t address, uint8_t value) {
	switch (address & 0x0003) {
	case 0x0000:
		VRC2::write_8(0x8000, value);
		break;
	case 0x0001:
		VRC2::write_8(0x8002, value);
		break;
	case 0x0002:
		VRC2::write_8(0x8001, value);
		break;
	case 0x0003:
		VRC2::write_8(0x8003, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_9(uint_least16_t address, uint8_t value) {
	switch (address & 0x0003) {
	case 0x0000:
		VRC2::write_9(0x9000, value);
		break;
	case 0x0001:
		VRC2::write_9(0x9002, value);
		break;
	case 0x0002:
		VRC2::write_9(0x9001, value);
		break;
	case 0x0003:
		VRC2::write_9(0x9003, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_a(uint_least16_t address, uint8_t value) {
	switch (address & 0x0003) {
	case 0x0000:
		VRC2::write_a(0xa000, value);
		break;
	case 0x0001:
		VRC2::write_a(0xa002, value);
		break;
	case 0x0002:
		VRC2::write_a(0xa001, value);
		break;
	case 0x0003:
		VRC2::write_a(0xa003, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_b(uint_least16_t address, uint8_t value) {
	switch (address & 0x0003) {
	case 0x0000:
		VRC2::write_b(0xb000, value);
		break;
	case 0x0001:
		VRC2::write_b(0xb002, value);
		break;
	case 0x0002:
		VRC2::write_b(0xb001, value);
		break;
	case 0x0003:
		VRC2::write_b(0xb003, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_c(uint_least16_t address, uint8_t value) {
	switch (address & 0x0003) {
	case 0x0000:
		VRC2::write_c(0xc000, value);
		break;
	case 0x0001:
		VRC2::write_c(0xc002, value);
		break;
	case 0x0002:
		VRC2::write_c(0xc001, value);
		break;
	case 0x0003:
		VRC2::write_c(0xc003, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_d(uint_least16_t address, uint8_t value) {
	switch (address & 0x0003) {
	case 0x0000:
		VRC2::write_d(0xd000, value);
		break;
	case 0x0001:
		VRC2::write_d(0xd002, value);
		break;
	case 0x0002:
		VRC2::write_d(0xd001, value);
		break;
	case 0x0003:
		VRC2::write_d(0xd003, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_e(uint_least16_t address, uint8_t value) {
	switch (address & 0x0003) {
	case 0x0000:
		VRC2::write_e(0xe000, value);
		break;
	case 0x0001:
		VRC2::write_e(0xe002, value);
		break;
	case 0x0002:
		VRC2::write_e(0xe001, value);
		break;
	case 0x0003:
		VRC2::write_e(0xe003, value);
		break;
	}
}
