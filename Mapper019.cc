
#include "Mapper019.h"
#include <cstring>

// TODO: CHR-RAM swapping

SETUP_STATIC_INES_MAPPER_REGISTRAR(19);

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper19::Mapper19() : irq_counter_(0), mirroring_(0), irq_enabled_(false) {

	memset(prg_ram_, 0, sizeof(prg_ram_));
	memset(chr_ram_, 0, sizeof(chr_ram_));

	set_prg_89abcdef(0);
	set_prg_ef(-1);

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper19::name() const {
	return "Namcot 106";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper19::read_4(uint16_t address) {
	switch(address & 0xf800) {
	case 0x4800:
		return 0;
	default:
		return Mapper::read_4(address);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper19::read_5(uint16_t address) {

	switch(address & 0xf800) {
	case 0x5000:
		nes::cpu.clear_irq(CPU::MAPPER_IRQ);
		return irq_counter_ & 0x00ff;
	case 0x5800:
		nes::cpu.clear_irq(CPU::MAPPER_IRQ);
		return ((irq_counter_ & (0xff00 >> 8)) & 0x7f) | (irq_enabled_ ? 0x80 : 0x00);
	default:
		return Mapper::read_5(address);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper19::read_6(uint16_t address) {
	return prg_ram_[address & 0x1fff];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper19::read_7(uint16_t address) {
	return prg_ram_[address & 0x1fff];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_4(uint16_t address, uint8_t value) {
	switch(address & 0xf800) {
	case 0x4800:
		break;
	default:
		Mapper::write_4(address, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_5(uint16_t address, uint8_t value) {

	switch(address & 0xf800) {
	case 0x5000:
		// Low byte of IRQ counter
		irq_counter_ = (irq_counter_ & 0xff00) | value;
		nes::cpu.clear_irq(CPU::MAPPER_IRQ);
		break;
	case 0x5800:
		// High bits of IRQ counter
		irq_counter_ = (irq_counter_ & 0x00ff) | ((value << 8) & 0x7f00);
		irq_enabled_ = (value & 0x80);
		nes::cpu.clear_irq(CPU::MAPPER_IRQ);
		break;
	default:
		Mapper::write_5(address, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_6(uint16_t address, uint8_t value) {
	prg_ram_[address & 0x1fff] = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_7(uint16_t address, uint8_t value) {
	prg_ram_[address & 0x1fff] = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_8(uint16_t address, uint8_t value) {
	if(address & 0x0800) {
		// Select 1K VROM bank at PPU $0400
		set_chr_0400_07ff(value);
	} else {
		// Select 1K VROM bank at PPU $0000
		set_chr_0000_03ff(value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_9(uint16_t address, uint8_t value) {
	if(address & 0x0800) {
		// Select 1K VROM bank at PPU $0C00
		set_chr_0c00_0fff(value);
	} else {
		// Select 1K VROM bank at PPU $0800
		set_chr_0800_0bff(value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_a(uint16_t address, uint8_t value) {
	if(address & 0x0800) {
		// Select 1K VROM bank at PPU $1400
		set_chr_1400_17ff(value);
	} else {
		// Select 1K VROM bank at PPU $1000
		set_chr_1000_13ff(value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_b(uint16_t address, uint8_t value) {
	if(address & 0x0800) {
		// Select 1K VROM bank at PPU $1C00
		set_chr_1c00_1fff(value);
	} else {
		// Select 1K VROM bank at PPU $1800
		set_chr_1800_1bff(value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_c(uint16_t address, uint8_t value) {
	(void)value;
	if(address & 0x0800) {
		// Select 1K VROM bank at PPU $2400
		// A value of $E0 or above will
		// use VRAM instead
		mirroring_ = (mirroring_ & 0xf3) | ((value & 0x01) << 2);
		nes::ppu.set_mirroring(mirroring_);
	} else {
		// Select 1K VROM bank at PPU $2000
		// A value of $E0 or above will
		// use VRAM instead
		mirroring_ = (mirroring_ & 0xfc) | (value & 0x01);
		nes::ppu.set_mirroring(mirroring_);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_d(uint16_t address, uint8_t value) {
	(void)value;
	if(address & 0x0800) {
		// Select 1K VROM bank at PPU $2c00
		// A value of $E0 or above will
		// use VRAM instead
		mirroring_ = (mirroring_ & 0x3f) | ((value & 0x01) << 6);
		nes::ppu.set_mirroring(mirroring_);
	} else {
		//Select 1K VROM bank at PPU $2800
		//A value of $E0 or above will
		//use VRAM instead
		mirroring_ = (mirroring_ & 0xcf) | ((value & 0x01) << 4);
		nes::ppu.set_mirroring(mirroring_);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_e(uint16_t address, uint8_t value) {
	if(address & 0x0800) {
		set_prg_ab(value);
	} else {
		set_prg_89(value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::write_f(uint16_t address, uint8_t value) {
	if(!(address & 0x0800)) {
		set_prg_cd(value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper19::cpu_sync() {
	if(irq_counter_ == 0x7fff) {
		nes::cpu.irq(CPU::MAPPER_IRQ);
	}
	irq_counter_ = (irq_counter_ + 1) & 0x7fff;
}
