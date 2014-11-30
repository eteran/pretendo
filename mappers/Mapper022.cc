
#include "Mapper022.h"
#include "VRC2.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(22)

//------------------------------------------------------------------------------
// Name: Mapper22
//------------------------------------------------------------------------------
Mapper22::Mapper22() : mapper_(new VRC2(1)) {

}

//------------------------------------------------------------------------------
// Name: ~Mapper22
//------------------------------------------------------------------------------
Mapper22::~Mapper22() {
	delete mapper_;
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
uint8_t Mapper22::read_6(uint16_t address) {
	switch(address & 0xf003) {
	case 0x6000: return mapper_->read_6(0x6000);
	case 0x6001: return mapper_->read_6(0x6002);
	case 0x6002: return mapper_->read_6(0x6001);
	case 0x6003: return mapper_->read_6(0x6003);
	}

	return -1;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_6(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0x6000: mapper_->write_6(0x6000, value); break;
	case 0x6001: mapper_->write_6(0x6002, value); break;
	case 0x6002: mapper_->write_6(0x6001, value); break;
	case 0x6003: mapper_->write_6(0x6003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_8(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0x8000: mapper_->write_8(0x8000, value); break;
	case 0x8001: mapper_->write_8(0x8002, value); break;
	case 0x8002: mapper_->write_8(0x8001, value); break;
	case 0x8003: mapper_->write_8(0x8003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_9(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0x9000: mapper_->write_9(0x9000, value); break;
	case 0x9001: mapper_->write_9(0x9002, value); break;
	case 0x9002: mapper_->write_9(0x9001, value); break;
	case 0x9003: mapper_->write_9(0x9003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_a(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xa000: mapper_->write_a(0xa000, value); break;
	case 0xa001: mapper_->write_a(0xa002, value); break;
	case 0xa002: mapper_->write_a(0xa001, value); break;
	case 0xa003: mapper_->write_a(0xa003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_b(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xb000: mapper_->write_b(0xb000, value); break;
	case 0xb001: mapper_->write_b(0xb002, value); break;
	case 0xb002: mapper_->write_b(0xb001, value); break;
	case 0xb003: mapper_->write_b(0xb003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_c(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xc000: mapper_->write_c(0xc000, value); break;
	case 0xc001: mapper_->write_c(0xc002, value); break;
	case 0xc002: mapper_->write_c(0xc001, value); break;
	case 0xc003: mapper_->write_c(0xc003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_d(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xd000: mapper_->write_d(0xd000, value); break;
	case 0xd001: mapper_->write_d(0xd002, value); break;
	case 0xd002: mapper_->write_d(0xd001, value); break;
	case 0xd003: mapper_->write_d(0xd003, value); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper22::write_e(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xe000: mapper_->write_e(0xe000, value); break;
	case 0xe001: mapper_->write_e(0xe002, value); break;
	case 0xe002: mapper_->write_e(0xe001, value); break;
	case 0xe003: mapper_->write_e(0xe003, value); break;
	}
}
