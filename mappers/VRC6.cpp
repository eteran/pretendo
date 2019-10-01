
#include "VRC6.h"
#include "Cpu.h"
#include "Ppu.h"
#include <cstring>

//------------------------------------------------------------------------------
// Name: VRC6
//------------------------------------------------------------------------------
VRC6::VRC6()  {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string VRC6::name() const {
	return "VRC6";
}

//------------------------------------------------------------------------------
// Name: write_8
//------------------------------------------------------------------------------
void VRC6::write_8(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		set_prg_89ab(value & 0x0f);
		break;
	}
}

//------------------------------------------------------------------------------
// Name: write_9
//------------------------------------------------------------------------------
void VRC6::write_9(uint16_t address, uint8_t value) {

	(void)value;

	switch(address & 0xf003) {
	case 0x9000:
		// pulse 1 duty & volume
		break;
	case 0x9001:
		// pulse 1 period low
		break;
	case 0x9002:
		// pulse 1 period high
		break;
	case 0x9003:
		// Frequency scaling
		break;
	}
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void VRC6::write_a(uint16_t address, uint8_t value) {

	(void)value;

	switch(address & 0xf003) {
	case 0xa000:
		// pulse2 duty & volume
		break;
	case 0xa001:
		// pulse2 period low
		break;
	case 0xa002:
		// pulse2 period high
		break;
	}
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void VRC6::write_b(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xb000:
		// saw volume
		break;
	case 0xb001:
		// saw period low
		break;
	case 0xb002:
		// saw period high
		break;
	case 0xb003:
		switch((value >> 2) & 0x03) {
		case 0x00: set_mirroring(nes::ppu::mirror_vertical);    break;
		case 0x01: set_mirroring(nes::ppu::mirror_horizontal);  break;
		case 0x02: set_mirroring(nes::ppu::mirror_single_low);  break;
		case 0x03: set_mirroring(nes::ppu::mirror_single_high); break;
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void VRC6::write_c(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xc000:
	case 0xc001:
	case 0xc002:
	case 0xc003:
		set_prg_cd(value & 0x1f);
		break;
	}
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void VRC6::write_d(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xd000: set_chr_0000_03ff(value); break;
	case 0xd001: set_chr_0400_07ff(value); break;
	case 0xd002: set_chr_0800_0bff(value); break;
	case 0xd003: set_chr_0c00_0fff(value); break;
	}
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void VRC6::write_e(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xe000: set_chr_1000_13ff(value); break;
	case 0xe001: set_chr_1400_17ff(value); break;
	case 0xe002: set_chr_1800_1bff(value); break;
	case 0xe003: set_chr_1c00_1fff(value); break;
	}
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void VRC6::write_f(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xf000:
		irq_latch_ = value;
		break;

	case 0xf001:
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		irq_control_.raw = value;
		if(irq_control_.enabled) {
			irq_counter_   = irq_latch_;
			irq_prescaler_ = 341;
		}
		break;

	case 0xf002:
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		irq_control_.enabled = irq_control_.a;
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC6::cpu_sync() {

	if(irq_control_.enabled) {
		if(irq_control_.mode) {
			clock_irq();
		} else {
			irq_prescaler_ -= 3;
			if(irq_prescaler_ <= 0) {
				clock_irq();
				irq_prescaler_ += 341;
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name: clock_irq
//------------------------------------------------------------------------------
void VRC6::clock_irq() {

	if(irq_counter_ == 0xff) {
		irq_counter_ = irq_latch_;
		nes::cpu::irq(nes::cpu::MAPPER_IRQ);
	} else {
		++irq_counter_;
	}
}
