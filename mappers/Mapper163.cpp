
#include "Mapper163.h"
#include "Cart.h"
#include "NES.h"
#include <cstring>
#include <iostream>

SETUP_STATIC_INES_MAPPER_REGISTRAR(163)

//------------------------------------------------------------------------------
// Name: Mapper163
//------------------------------------------------------------------------------
Mapper163::Mapper163() {

    prg_ptr_ = open_sram(0x2000);

	set_prg_89abcdef(0);
	
	memset(chr_ram_, 0, sizeof(chr_ram_));

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper163::name() const {
	return "iNES 163";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper163::read_5(uint16_t address) {
	switch(address & 0x7300) {
	case 0x5100:
		return security_;


	case 0x5500:
		if(trigger_) {
			return security_;
		} else {
			return 0;
		}
	}

	return Mapper::read_5(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper163::read_6(uint16_t address) {
	return prg_ptr_[address & 0x1fff];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper163::read_7(uint16_t address) {
	return prg_ptr_[address & 0x1fff];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper163::write_5(uint16_t address, uint8_t value) {

	switch(address & 0x7300) {
	case 0x5000:
		prg_ = (prg_ & 0xf0) | (value & 0x0f);
		set_prg_89abcdef(prg_);
		break;

	case 0x5100:
		if(value == 6) {
			set_prg_89abcdef(3);
		}
		break;

	case 0x5200:
		prg_ = (prg_ & 0x0f) | ((value << 4) & 0xf0);
		set_prg_89abcdef(prg_);
		break;

	case 0x5300:
		security_ = value;
		break;
	}


	switch(address & 0x7301) {
	case 0x5101:
		if(reg_5101_ != 0 && value == 0) {
			trigger_ ^= 1;
		}

		reg_5101_ = value;
	}

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper163::write_6(uint16_t address, uint8_t value) {
	prg_ptr_[address & 0x1fff] = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper163::write_7(uint16_t address, uint8_t value) {
	prg_ptr_[address & 0x1fff] = value;
}
