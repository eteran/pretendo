
#include "Mapper021.h"
#include "VRC4.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(21)

//------------------------------------------------------------------------------
// Name: Mapper21
//------------------------------------------------------------------------------
Mapper21::Mapper21() : mapper_(new VRC4) {
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper21::name() const {
	return "VRC4a";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_8(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0x8000: mapper_->write_8(0x8000, value); break;
	case 0x8002: mapper_->write_8(0x8002, value); break;
	case 0x8004: mapper_->write_8(0x8004, value); break;
	case 0x8006: mapper_->write_8(0x8006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_9(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0x9000: mapper_->write_9(0x9000, value); break;
	case 0x9002: mapper_->write_9(0x9002, value); break;
	case 0x9004: mapper_->write_9(0x9004, value); break;
	case 0x9006: mapper_->write_9(0x9006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_a(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xa000: mapper_->write_a(0xa000, value); break;
	case 0xa002: mapper_->write_a(0xa002, value); break;
	case 0xa004: mapper_->write_a(0xa004, value); break;
	case 0xa006: mapper_->write_a(0xa006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_b(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xb000: mapper_->write_b(0xb000, value); break;
	case 0xb002: mapper_->write_b(0xb002, value); break;
	case 0xb004: mapper_->write_b(0xb004, value); break;
	case 0xb006: mapper_->write_b(0xb006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_c(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xc000: mapper_->write_c(0xc000, value); break;
	case 0xc002: mapper_->write_c(0xc002, value); break;
	case 0xc004: mapper_->write_c(0xc004, value); break;
	case 0xc006: mapper_->write_c(0xc006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_d(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xd000: mapper_->write_d(0xd000, value); break;
	case 0xd002: mapper_->write_d(0xd002, value); break;
	case 0xd004: mapper_->write_d(0xd004, value); break;
	case 0xd006: mapper_->write_d(0xd006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_e(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xe000: mapper_->write_e(0xe000, value); break;
	case 0xe002: mapper_->write_e(0xe002, value); break;
	case 0xe004: mapper_->write_e(0xe004, value); break;
	case 0xe006: mapper_->write_e(0xe006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::write_f(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xf000: mapper_->write_f(0xf000, value); break;
	case 0xf002: mapper_->write_f(0xf002, value); break;
	case 0xf004: mapper_->write_f(0xf004, value); break;
	case 0xf006: mapper_->write_f(0xf006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper21::cpu_sync() {
	mapper_->cpu_sync();
}
