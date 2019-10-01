
#include "Mapper025.h"
#include "VRC2.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(25)

//------------------------------------------------------------------------------
// Name: Mapper25
//------------------------------------------------------------------------------
Mapper25::Mapper25() : VRC2(0) {
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper25::name() const {
	return "VRC2c";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper25::read_6(uint16_t address) {
	switch(address & 0x0003) {
	case 0x0000: return VRC2::read_6(0x6000);
	case 0x0001: return VRC2::read_6(0x6002);
	case 0x0002: return VRC2::read_6(0x6001);
	case 0x0003: return VRC2::read_6(0x6003);
	}

	return 0xff;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper25::write_6(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC2::write_6(0x6000, value); break;
	case 0x0001: VRC2::write_6(0x6002, value); break;
	case 0x0002: VRC2::write_6(0x6001, value); break;
	case 0x0003: VRC2::write_6(0x6003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper25::write_8(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC2::write_8(0x8000, value); break;
	case 0x0001: VRC2::write_8(0x8002, value); break;
	case 0x0002: VRC2::write_8(0x8001, value); break;
	case 0x0003: VRC2::write_8(0x8003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper25::write_9(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC2::write_9(0x9000, value); break;
	case 0x0001: VRC2::write_9(0x9002, value); break;
	case 0x0002: VRC2::write_9(0x9001, value); break;
	case 0x0003: VRC2::write_9(0x9003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper25::write_a(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC2::write_a(0xa000, value); break;
	case 0x0001: VRC2::write_a(0xa002, value); break;
	case 0x0002: VRC2::write_a(0xa001, value); break;
	case 0x0003: VRC2::write_a(0xa003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper25::write_b(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC2::write_b(0xb000, value); break;
	case 0x0001: VRC2::write_b(0xb002, value); break;
	case 0x0002: VRC2::write_b(0xb001, value); break;
	case 0x0003: VRC2::write_b(0xb003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper25::write_c(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC2::write_c(0xc000, value); break;
	case 0x0001: VRC2::write_c(0xc002, value); break;
	case 0x0002: VRC2::write_c(0xc001, value); break;
	case 0x0003: VRC2::write_c(0xc003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper25::write_d(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC2::write_d(0xd000, value); break;
	case 0x0001: VRC2::write_d(0xd002, value); break;
	case 0x0002: VRC2::write_d(0xd001, value); break;
	case 0x0003: VRC2::write_d(0xd003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper25::write_e(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC2::write_e(0xe000, value); break;
	case 0x0001: VRC2::write_e(0xe002, value); break;
	case 0x0002: VRC2::write_e(0xe001, value); break;
	case 0x0003: VRC2::write_e(0xe003, value); break;
	}
}
