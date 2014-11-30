
#include "Mapper027.h"
#include "VRC4.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(27)

//------------------------------------------------------------------------------
// Name: Mapper27
//------------------------------------------------------------------------------
Mapper27::Mapper27() : mapper_(new VRC4) {
}

//------------------------------------------------------------------------------
// Name: ~Mapper27
//------------------------------------------------------------------------------
Mapper27::~Mapper27() {
	delete mapper_;
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper27::name() const {
	return "VRC4a";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::write_8(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0x8000: mapper_->write_8(0x8000, value); break;
	case 0x8001: mapper_->write_8(0x8002, value); break;
	case 0x8002: mapper_->write_8(0x8004, value); break;
	case 0x8003: mapper_->write_8(0x8006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::write_9(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0x9000: mapper_->write_9(0x9000, value); break;
	case 0x9001: mapper_->write_9(0x9002, value); break;
	case 0x9002: mapper_->write_9(0x9004, value); break;
	case 0x9003: mapper_->write_9(0x9006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::write_a(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xa000: mapper_->write_a(0xa000, value); break;
	case 0xa001: mapper_->write_a(0xa002, value); break;
	case 0xa002: mapper_->write_a(0xa004, value); break;
	case 0xa003: mapper_->write_a(0xa006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::write_b(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xb000: mapper_->write_b(0xb000, value); break;
	case 0xb001: mapper_->write_b(0xb002, value); break;
	case 0xb002: mapper_->write_b(0xb004, value); break;
	case 0xb003: mapper_->write_b(0xb006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::write_c(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xc000: mapper_->write_c(0xc000, value); break;
	case 0xc001: mapper_->write_c(0xc002, value); break;
	case 0xc002: mapper_->write_c(0xc004, value); break;
	case 0xc003: mapper_->write_c(0xc006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::write_d(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xd000: mapper_->write_d(0xd000, value); break;
	case 0xd001: mapper_->write_d(0xd002, value); break;
	case 0xd002: mapper_->write_d(0xd004, value); break;
	case 0xd003: mapper_->write_d(0xd006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::write_e(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xe000: mapper_->write_e(0xe000, value); break;
	case 0xe001: mapper_->write_e(0xe002, value); break;
	case 0xe002: mapper_->write_e(0xe004, value); break;
	case 0xe003: mapper_->write_e(0xe006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::write_f(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xf000: mapper_->write_f(0xf000, value); break;
	case 0xf001: mapper_->write_f(0xf002, value); break;
	case 0xf002: mapper_->write_f(0xf004, value); break;
	case 0xf003: mapper_->write_f(0xf006, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper27::cpu_sync() {
	mapper_->cpu_sync();
}
