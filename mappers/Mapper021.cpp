
#include "Mapper021.h"
#include "VRC4.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(21)

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper21::name() const {
	return "VRC4a";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_8(uint32_t address, uint8_t value) {
	switch (address & 0x000c) {
	case 0x0000:
		VRC4::write_8(0x8000, value);
		break;
	case 0x0002:
		VRC4::write_8(0x8002, value);
		break;
	case 0x0004:
		VRC4::write_8(0x8004, value);
		break;
	case 0x0006:
		VRC4::write_8(0x8006, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_9(uint32_t address, uint8_t value) {
	switch (address & 0x000c) {
	case 0x0000:
		VRC4::write_9(0x9000, value);
		break;
	case 0x0002:
		VRC4::write_9(0x9002, value);
		break;
	case 0x0004:
		VRC4::write_9(0x9004, value);
		break;
	case 0x0006:
		VRC4::write_9(0x9006, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_a(uint32_t address, uint8_t value) {
	switch (address & 0x000c) {
	case 0x0000:
		VRC4::write_a(0xa000, value);
		break;
	case 0x0002:
		VRC4::write_a(0xa002, value);
		break;
	case 0x0004:
		VRC4::write_a(0xa004, value);
		break;
	case 0x0006:
		VRC4::write_a(0xa006, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_b(uint32_t address, uint8_t value) {
	switch (address & 0x000c) {
	case 0x0000:
		VRC4::write_b(0xb000, value);
		break;
	case 0x0002:
		VRC4::write_b(0xb002, value);
		break;
	case 0x0004:
		VRC4::write_b(0xb004, value);
		break;
	case 0x0006:
		VRC4::write_b(0xb006, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_c(uint32_t address, uint8_t value) {
	switch (address & 0x000c) {
	case 0x0000:
		VRC4::write_c(0xc000, value);
		break;
	case 0x0002:
		VRC4::write_c(0xc002, value);
		break;
	case 0x0004:
		VRC4::write_c(0xc004, value);
		break;
	case 0x0006:
		VRC4::write_c(0xc006, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_d(uint32_t address, uint8_t value) {
	switch (address & 0x000c) {
	case 0x0000:
		VRC4::write_d(0xd000, value);
		break;
	case 0x0002:
		VRC4::write_d(0xd002, value);
		break;
	case 0x0004:
		VRC4::write_d(0xd004, value);
		break;
	case 0x0006:
		VRC4::write_d(0xd006, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_e(uint32_t address, uint8_t value) {
	switch (address & 0x000c) {
	case 0x000:
		VRC4::write_e(0xe000, value);
		break;
	case 0x0002:
		VRC4::write_e(0xe002, value);
		break;
	case 0x0004:
		VRC4::write_e(0xe004, value);
		break;
	case 0x0006:
		VRC4::write_e(0xe006, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_f(uint32_t address, uint8_t value) {
	switch (address & 0x000c) {
	case 0x0000:
		VRC4::write_f(0xf000, value);
		break;
	case 0x0002:
		VRC4::write_f(0xf002, value);
		break;
	case 0x0004:
		VRC4::write_f(0xf004, value);
		break;
	case 0x0006:
		VRC4::write_f(0xf006, value);
		break;
	}
}
