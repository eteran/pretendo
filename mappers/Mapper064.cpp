
#include "Mapper064.h"
#include "Ppu.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(64)

namespace {
enum {
	PRG_6 = 0,
	PRG_7 = 1,
	PRG_F = 2,

	CHR_0 = 0,
	CHR_1 = 1,
	CHR_2 = 2,
	CHR_3 = 3,
	CHR_4 = 4,
	CHR_5 = 5,
	CHR_8 = 6,
	CHR_9 = 7
};
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper64::Mapper64() {

	prg_bank_[PRG_6] = 0;
	prg_bank_[PRG_7] = 1;
	prg_bank_[PRG_F] = -2;

	chr_bank_[CHR_0] = 0x00;
	chr_bank_[CHR_1] = 0x01;
	chr_bank_[CHR_2] = 0x02;
	chr_bank_[CHR_3] = 0x03;
	chr_bank_[CHR_4] = 0x04;
	chr_bank_[CHR_5] = 0x05;
	chr_bank_[CHR_8] = 0x06;
	chr_bank_[CHR_9] = 0x07;

	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::sync_prg() {
	if ((command_ & 0x40) != 0) {
		set_prg_89(prg_bank_[PRG_F]);
		set_prg_ab(prg_bank_[PRG_6]);
		set_prg_cd(prg_bank_[PRG_7]);
	} else {
		set_prg_89(prg_bank_[PRG_6]);
		set_prg_ab(prg_bank_[PRG_7]);
		set_prg_cd(prg_bank_[PRG_F]);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::sync_chr() {
	switch (command_ & 0xa0) {
	case 0x00:
		set_chr_0000_03ff((chr_bank_[CHR_0] & 0xfe) | 0x00);
		set_chr_0400_07ff((chr_bank_[CHR_0] & 0xfe) | 0x01);
		set_chr_0800_0bff((chr_bank_[CHR_1] & 0xfe) | 0x00);
		set_chr_0c00_0fff((chr_bank_[CHR_1] & 0xfe) | 0x01);
		set_chr_1000_13ff(chr_bank_[CHR_2]);
		set_chr_1400_17ff(chr_bank_[CHR_3]);
		set_chr_1800_1bff(chr_bank_[CHR_4]);
		set_chr_1c00_1fff(chr_bank_[CHR_5]);
		break;
	case 0x20:
		set_chr_0000_03ff(chr_bank_[CHR_0]);
		set_chr_0400_07ff(chr_bank_[CHR_8]);
		set_chr_0800_0bff(chr_bank_[CHR_1]);
		set_chr_0c00_0fff(chr_bank_[CHR_9]);
		set_chr_1000_13ff(chr_bank_[CHR_2]);
		set_chr_1400_17ff(chr_bank_[CHR_3]);
		set_chr_1800_1bff(chr_bank_[CHR_4]);
		set_chr_1c00_1fff(chr_bank_[CHR_5]);
		break;
	case 0x80:
		set_chr_0000_03ff(chr_bank_[CHR_2]);
		set_chr_0400_07ff(chr_bank_[CHR_3]);
		set_chr_0800_0bff(chr_bank_[CHR_4]);
		set_chr_0c00_0fff(chr_bank_[CHR_5]);
		set_chr_1000_13ff((chr_bank_[CHR_0] & 0xfe) | 0x00);
		set_chr_1400_17ff((chr_bank_[CHR_0] & 0xfe) | 0x01);
		set_chr_1800_1bff((chr_bank_[CHR_1] & 0xfe) | 0x00);
		set_chr_1c00_1fff((chr_bank_[CHR_1] & 0xfe) | 0x01);
		break;
	case 0xa0:
		set_chr_0000_03ff(chr_bank_[CHR_2]);
		set_chr_0400_07ff(chr_bank_[CHR_3]);
		set_chr_0800_0bff(chr_bank_[CHR_4]);
		set_chr_0c00_0fff(chr_bank_[CHR_5]);
		set_chr_1000_13ff(chr_bank_[CHR_0]);
		set_chr_1400_17ff(chr_bank_[CHR_8]);
		set_chr_1800_1bff(chr_bank_[CHR_1]);
		set_chr_1c00_1fff(chr_bank_[CHR_9]);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper64::name() const {
	return "Tengen RAMBO-1";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::write_8(uint32_t address, uint8_t value) {

	switch (address & 0x0001) {
	case 0x0000:
		command_ = value;
		break;

	case 0x0001:
		switch (command_ & 0x0f) {
		case 0x00:
			chr_bank_[CHR_0] = value;
			break;
		case 0x01:
			chr_bank_[CHR_1] = value;
			break;
		case 0x02:
			chr_bank_[CHR_2] = value;
			break;
		case 0x03:
			chr_bank_[CHR_3] = value;
			break;
		case 0x04:
			chr_bank_[CHR_4] = value;
			break;
		case 0x05:
			chr_bank_[CHR_5] = value;
			break;
		case 0x06:
			prg_bank_[PRG_6] = value;
			break;
		case 0x07:
			prg_bank_[PRG_7] = value;
			break;
		case 0x08:
			chr_bank_[CHR_8] = value;
			break;
		case 0x09:
			chr_bank_[CHR_9] = value;
			break;
		case 0x0f:
			prg_bank_[PRG_F] = value;
			break;
		}
		break;
	}

	sync_prg();
	sync_chr();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::write_9(uint32_t address, uint8_t value) {
	write_8(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::write_a(uint32_t address, uint8_t value) {
	switch (address & 1) {
	case 0x0000:
		if (value & 0x01) {
			set_mirroring(mirror_horizontal);
		} else {
			set_mirroring(mirror_vertical);
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::write_b(uint32_t address, uint8_t value) {
	write_a(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::write_c(uint32_t address, uint8_t value) {
	switch (address & 0x0001) {
	case 0x0000:
		irq_latch_ = value;
		break;
	case 0x0001:
		irq_counter_ = 0x00;
		irq_reload_  = true;
		irq_mode_    = value & 0x01;
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::write_d(uint32_t address, uint8_t value) {
	write_c(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::write_e(uint32_t address, uint8_t value) {
	(void)value;

	switch (address & 0x0001) {
	case 0x0000:
		irq_enabled_ = false;
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		break;
	case 0x0001:
		irq_enabled_ = true;
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::write_f(uint32_t address, uint8_t value) {
	write_e(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::vram_change_hook(uint32_t vram_address) {

	if (!irq_mode_) {
		if (vram_address & 0x1000 && !(prev_vram_address_ & 0x1000)) {
			if ((nes::ppu::cycle_count() - prev_ppu_cycle_) >= 16) {
				clock_irq();
			}

			prev_ppu_cycle_ = nes::ppu::cycle_count();
		}

		prev_vram_address_ = vram_address;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::clock_irq() {

	/*
	 * When the counter is clocked, if it's not zero, it is decremented,
	 * otherwise it is reloaded with the last value written to $C000. *After*
	 * decrementing/reloading, if the counter is zero and IRQ is enabled via
	 * $E001, the IRQ flag is set.
	 */

	if (irq_counter_ == 0x00) {
		irq_counter_ = irq_latch_ + 1;
	} else {
		--irq_counter_;
		irq_reload_ = true;
	}

	if (irq_enabled_ && irq_counter_ == 0) {
		nes::cpu::irq(nes::cpu::MAPPER_IRQ);
	}

	irq_reload_ = false;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper64::cpu_sync() {
	if (irq_mode_) {
		cpu_cycles_ = (cpu_cycles_ + 1) % 4;

		if (cpu_cycles_ == 0) {
			clock_irq();
		}
	}
}
