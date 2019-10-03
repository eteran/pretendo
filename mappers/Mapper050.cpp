
#include "Mapper050.h"
#include "Cart.h"
#include "Nes.h"
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(50)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper50::Mapper50() {

	set_prg_67(0x0f);
	set_prg_89(0x08);
	set_prg_ab(0x09);
	set_prg_cd(0);
	set_prg_ef(0x0b);

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper50::name() const {
	return "SMB2j Pirate";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper50::write_4(uint16_t address, uint8_t value) {
	if((address & 0xe060) == 0x4020) {
		write_handler(address, value);
	} else {
		Mapper::write_4(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper50::write_5(uint16_t address, uint8_t value) {
	if((address & 0xe060) == 0x4020) {
		write_handler(address, value);
	} else {
		Mapper::write_5(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper50::write_handler(uint16_t address, uint8_t value) {

	assert((address & 0xe060) == 0x4020);

	switch(address & 0x0100) {
	case 0x0000:
		// ROM page
		set_prg_cd(
			((value & 0x01) << 2) |
			((value & 0x02) >> 1) |
			((value & 0x04) >> 1) |
			((value & 0x08) >> 0));
		break;
	case 0x0100:
		// IRQ Timer
		if(value == 0) {
			irq_enabled_ = false;
			irq_counter_ = 0;
			nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		} else {
			irq_enabled_ = true;
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper50::cpu_sync() {
	if(irq_enabled_) {
		irq_counter_ = (irq_counter_ + 1) % 0x1000;
		if(irq_counter_ == 0) {
			nes::cpu::irq(nes::cpu::MAPPER_IRQ);
		}
	}
}
