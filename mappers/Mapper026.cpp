
#include "Mapper026.h"
#include "VRC6.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(26)

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper26::name() const {
	return "VRC6b";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper26::write_8(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC6::write_8(0x8000, value); break;
	case 0x0001: VRC6::write_8(0x8002, value); break;
	case 0x0002: VRC6::write_8(0x8001, value); break;
	case 0x0003: VRC6::write_8(0x8003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper26::write_9(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC6::write_9(0x9000, value); break;
	case 0x0001: VRC6::write_9(0x9002, value); break;
	case 0x0002: VRC6::write_9(0x9001, value); break;
	case 0x0003: VRC6::write_9(0x9003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper26::write_a(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC6::write_a(0xa000, value); break;
	case 0x0001: VRC6::write_a(0xa002, value); break;
	case 0x0002: VRC6::write_a(0xa001, value); break;
	case 0x0003: VRC6::write_a(0xa003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper26::write_b(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC6::write_b(0xb000, value); break;
	case 0x0001: VRC6::write_b(0xb002, value); break;
	case 0x0002: VRC6::write_b(0xb001, value); break;
	case 0x0003: VRC6::write_b(0xb003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper26::write_c(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC6::write_c(0xc000, value); break;
	case 0x0001: VRC6::write_c(0xc002, value); break;
	case 0x0002: VRC6::write_c(0xc001, value); break;
	case 0x0003: VRC6::write_c(0xc003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper26::write_d(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC6::write_d(0xd000, value); break;
	case 0x0001: VRC6::write_d(0xd002, value); break;
	case 0x0002: VRC6::write_d(0xd001, value); break;
	case 0x0003: VRC6::write_d(0xd003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper26::write_e(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC6::write_e(0xe000, value); break;
	case 0x0001: VRC6::write_e(0xe002, value); break;
	case 0x0002: VRC6::write_e(0xe001, value); break;
	case 0x0003: VRC6::write_e(0xe003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper26::write_f(uint16_t address, uint8_t value) {
	switch(address & 0x0003) {
	case 0x0000: VRC6::write_f(0xf000, value); break;
	case 0x0001: VRC6::write_f(0xf002, value); break;
	case 0x0002: VRC6::write_f(0xf001, value); break;
	case 0x0003: VRC6::write_f(0xf003, value); break;
	}
}
