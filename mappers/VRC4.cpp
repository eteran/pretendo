
#include "VRC4.h"
#include "Cpu.h"

// TODO(eteran): implement sub-mapper madness...

//------------------------------------------------------------------------------
// Name: VRC4
//------------------------------------------------------------------------------
VRC4::VRC4() {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	set_chr_0000_1fff(0);

	chr_[0] = 0;
	chr_[1] = 1;
	chr_[2] = 2;
	chr_[3] = 3;
	chr_[4] = 4;
	chr_[5] = 5;
	chr_[6] = 6;
	chr_[7] = 7;
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string VRC4::name() const {
	return "VRC4";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC4::write_8(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0x8000:
	case 0x8002:
	case 0x8004:
	case 0x8006:
		prg_[0] = value;
		if(prg_mode_ & 0x02) {
			set_prg_cd(prg_[0] & 0x1f);
			set_prg_89(-2);
		} else {
			set_prg_89(prg_[0] & 0x1f);
			set_prg_cd(-2);
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC4::write_9(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0x9004:
	case 0x9006:
		prg_mode_ = value;
		if(prg_mode_ & 0x02) {
			set_prg_cd(prg_[0] & 0x1f);
			set_prg_89(-2);
		} else {
			set_prg_89(prg_[0] & 0x1f);
			set_prg_cd(-2);
		}
		break;

	case 0x9000:
	case 0x9002:
		switch(value & 0x03) {
		case 0x00: set_mirroring(mirror_vertical);    break;
		case 0x01: set_mirroring(mirror_horizontal);  break;
		case 0x02: set_mirroring(mirror_single_low);  break;
		case 0x03: set_mirroring(mirror_single_high); break;
		}
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC4::write_a(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xa000:
	case 0xa002:
	case 0xa004:
	case 0xa006:
		prg_[1] = value;
		set_prg_ab(prg_[1] & 0x1f);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC4::write_b(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xb000: chr_[0] = (chr_[0] & 0xf0) | ((value & 0x0f) << 0); set_chr_0000_03ff(chr_[0]); break;
	case 0xb002: chr_[0] = (chr_[0] & 0x0f) | ((value & 0x0f) << 4); set_chr_0000_03ff(chr_[0]); break;
	case 0xb004: chr_[1] = (chr_[1] & 0xf0) | ((value & 0x0f) << 0); set_chr_0400_07ff(chr_[1]); break;
	case 0xb006: chr_[1] = (chr_[1] & 0x0f) | ((value & 0x0f) << 4); set_chr_0400_07ff(chr_[1]); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC4::write_c(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xc000: chr_[2] = (chr_[2] & 0xf0) | ((value & 0x0f) << 0); set_chr_0800_0bff(chr_[2]); break;
	case 0xc002: chr_[2] = (chr_[2] & 0x0f) | ((value & 0x0f) << 4); set_chr_0800_0bff(chr_[2]); break;
	case 0xc004: chr_[3] = (chr_[3] & 0xf0) | ((value & 0x0f) << 0); set_chr_0c00_0fff(chr_[3]); break;
	case 0xc006: chr_[3] = (chr_[3] & 0x0f) | ((value & 0x0f) << 4); set_chr_0c00_0fff(chr_[3]); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC4::write_d(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xd000: chr_[4] = (chr_[4] & 0xf0) | ((value & 0x0f) << 0); set_chr_1000_13ff(chr_[4]); break;
	case 0xd002: chr_[4] = (chr_[4] & 0x0f) | ((value & 0x0f) << 4); set_chr_1000_13ff(chr_[4]); break;
	case 0xd004: chr_[5] = (chr_[5] & 0xf0) | ((value & 0x0f) << 0); set_chr_1400_17ff(chr_[5]); break;
	case 0xd006: chr_[5] = (chr_[5] & 0x0f) | ((value & 0x0f) << 4); set_chr_1400_17ff(chr_[5]); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC4::write_e(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xe000: chr_[6] = (chr_[6] & 0xf0) | ((value & 0x0f) << 0); set_chr_1800_1bff(chr_[6]); break;
	case 0xe002: chr_[6] = (chr_[6] & 0x0f) | ((value & 0x0f) << 4); set_chr_1800_1bff(chr_[6]); break;
	case 0xe004: chr_[7] = (chr_[7] & 0xf0) | ((value & 0x0f) << 0); set_chr_1c00_1fff(chr_[7]); break;
	case 0xe006: chr_[7] = (chr_[7] & 0x0f) | ((value & 0x0f) << 4); set_chr_1c00_1fff(chr_[7]); break;
	}
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void VRC4::write_f(uint16_t address, uint8_t value) {
	switch(address & 0xf00c) {
	case 0xf000:
		irq_latch_ = (irq_latch_ & 0xf0) | ((value & 0x0f) << 0);
		break;

	case 0xf002:
		irq_latch_ = (irq_latch_ & 0x0f) | ((value & 0x0f) << 4);
		break;

	case 0xf004:
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		irq_control_.raw = value;
		if(irq_control_.enabled) {
			irq_counter_   = irq_latch_;
			irq_prescaler_ = 341;
		}
		break;

	case 0xf006:
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		irq_control_.enabled = irq_control_.a;
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC4::cpu_sync() {

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
void VRC4::clock_irq() {

	if(irq_counter_ == 0xff) {
		irq_counter_ = irq_latch_;
		nes::cpu::irq(nes::cpu::MAPPER_IRQ);
	} else {
		++irq_counter_;
	}
}
