
#include "Mapper018.h"
#include "Nes.h"
#include "Cart.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(18)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper18::Mapper18() {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper18::name() const {
	return "JALECO SS88006";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_8(uint16_t address, uint8_t value) {

	switch(address & 0x000f) {
	case 0x0000:
	case 0x0001:
	case 0x0002:
	case 0x0003:
		prg_[address & 0x03] = value & 0x0f;
		break;
	}

	set_prg_89(prg_[0] | (prg_[1] << 4));
	set_prg_ab(prg_[2] | (prg_[3] << 4));
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_9(uint16_t address, uint8_t value) {

	switch(address & 0x000f) {
	case 0x0000:
	case 0x0001:
		prg_[0x04 | (address & 0x03)] = value & 0x0f;
		break;
	}

	set_prg_cd(prg_[4] | (prg_[5] << 4));
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_a(uint16_t address, uint8_t value) {
	chr_[0x00 | (address & 0x03)] = value & 0x0f;
	set_chr_0000_03ff(chr_[0x00] | (chr_[0x01] << 4));
	set_chr_0400_07ff(chr_[0x02] | (chr_[0x03] << 4));
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_b(uint16_t address, uint8_t value) {
	chr_[0x04 | (address & 0x03)] = value & 0x0f;
	set_chr_0800_0bff(chr_[0x04] | (chr_[0x05] << 4));
	set_chr_0c00_0fff(chr_[0x06] | (chr_[0x07] << 4));
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_c(uint16_t address, uint8_t value) {
	chr_[0x08 | (address & 0x03)] = value & 0x0f;
	set_chr_1000_13ff(chr_[0x08] | (chr_[0x09] << 4));
	set_chr_1400_17ff(chr_[0x0a] | (chr_[0x0b] << 4));
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_d(uint16_t address, uint8_t value) {
	chr_[0x0C | (address & 0x03)] = value & 0x0f;
	set_chr_1800_1bff(chr_[0x0c] | (chr_[0x0d] << 4));
	set_chr_1c00_1fff(chr_[0x0e] | (chr_[0x0f] << 4));
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_e(uint16_t address, uint8_t value) {
	switch(address & 0x000f) {
	case 0x0000:
		irq_latch_.byte1 = (value & 0x0f);
		break;
	case 0x0001:
		irq_latch_.byte2 = (value & 0x0f);
		break;
	case 0x0002:
		irq_latch_.byte3 = (value & 0x0f);
		break;
	case 0x0003:
		irq_latch_.byte4 = (value & 0x0f);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_f(uint16_t address, uint8_t value) {
	switch(address & 0x0fff) {
	case 0x0000:
		irq_counter_.raw = irq_latch_.raw;
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		break;

	case 0x0001:
		irq_control_.raw = value;
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		break;

	case 0x0002:
		switch(value & 0x03) {
		case 0x00: set_mirroring(mirror_horizontal);  break;
		case 0x01: set_mirroring(mirror_vertical);    break;
		case 0x02: set_mirroring(mirror_single_low);  break;
		case 0x03: set_mirroring(mirror_single_high); break;
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::cpu_sync() {

	bool wrap = false;
	if(irq_control_.mode_4bit) {
		wrap = (--irq_counter_.counter_4bit == 0);
	} else if(irq_control_.mode_8bit) {
		wrap = (--irq_counter_.counter_8bit == 0);
	} else if(irq_control_.mode_12bit) {
		wrap = (--irq_counter_.counter_12bit == 0);
	} else {
		wrap = (--irq_counter_.counter_16bit == 0);
	}

	if(wrap && irq_control_.enabled) {
		nes::cpu::irq(nes::cpu::MAPPER_IRQ);
	}
}
