
#include "Mapper018.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(18);

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper18::Mapper18() : irq_counter_(0), irq_latch_(0), irq_mask_(0), irq_control_(0) {

	memset(prg_, 0, sizeof(prg_));
	memset(chr_, 0, sizeof(chr_));
	memset(chr_ram_, 0, sizeof(chr_ram_));

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
		irq_latch_ = (irq_latch_ & 0xfff0) | (value & 0x0f);
		break;
	case 0x0001:
		irq_latch_ = (irq_latch_ & 0xff0f) | ((value & 0x0f) << 4);
		break;
	case 0x0002:
		irq_latch_ = (irq_latch_ & 0xf0ff) | ((value & 0x0f) << 8);
		break;
	case 0x0003:
		irq_latch_ = (irq_latch_ & 0x0fff) | ((value & 0x0f) << 12);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::write_f(uint16_t address, uint8_t value) {
	switch(address & 0x0fff) {
	case 0x0000:
		irq_counter_ = irq_latch_;
		nes::cpu.clear_irq(CPU::MAPPER_IRQ);
		break;

	case 0x0001:
		irq_control_ = value;

		switch(value & 0x0e) {
		case 0x00:
			irq_mask_ = 0xffff;
			break;
		case 0x04:
		case 0x06:
			irq_mask_ = 0x00ff;
			break;
		case 0x08:
		case 0x0a:
		case 0x0c:
		case 0x0e:
			irq_mask_ = 0x000f;
			break;
		default:
			irq_mask_ = 0x0000;
		}
		nes::cpu.clear_irq(CPU::MAPPER_IRQ);
		break;

	case 0x0002:
		switch(value & 0x03) {
		case 0x00: nes::ppu.set_mirroring(PPU::mirror_horizontal);  break;
		case 0x01: nes::ppu.set_mirroring(PPU::mirror_vertical);    break;
		case 0x02: nes::ppu.set_mirroring(PPU::mirror_single_low);  break;
		case 0x03: nes::ppu.set_mirroring(PPU::mirror_single_high); break;
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper18::cpu_sync() {
	const bool wrap = (irq_counter_ & irq_mask_) == 0;
	irq_counter_ = (irq_counter_ & ~irq_mask_) | ((irq_counter_ & irq_mask_) - 1);

	if(wrap && (irq_control_ & 0x01)) {
		nes::cpu.irq(CPU::MAPPER_IRQ);
	}
}
