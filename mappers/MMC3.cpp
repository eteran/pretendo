
#include "MMC3.h"
#include "PPU.h"
#include "Cart.h"
#include "NES.h"
#include <cstring>
#include <iostream>

//------------------------------------------------------------------------------
// Name: MMC3
//------------------------------------------------------------------------------
MMC3::MMC3() {

    prg_ptr_ = open_sram(0x2000);

	set_prg_89ab(0);
	set_prg_cdef(-1);

	memset(chr_ram_,  0, sizeof(chr_ram_));
	memset(chr_bank_, 0, sizeof(chr_bank_));
	memset(prg_bank_, 0, sizeof(prg_bank_));

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name: prg_bank
//------------------------------------------------------------------------------
int MMC3::prg_bank(int bank) const {

	const uint8_t banks[] = {
		prg_bank_[0],
		prg_bank_[1],
		static_cast<uint8_t>(-2),
		static_cast<uint8_t>(-1)
	};

	if(bank & 0x1) {
		return banks[bank];
	} else {
		return banks[bank ^ ((command_ & 0x40) >> 5)];
	}
}

//------------------------------------------------------------------------------
// Name: chr_bank
//------------------------------------------------------------------------------
int MMC3::chr_bank(int bank) const {
	return chr_bank_[bank ^ ((command_ & 0x80) >> 5)];
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string MMC3::name() const {
	return "MMC3";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t MMC3::read_6(uint16_t address) {

	if(save_ram_enabled_ && nes::cart.mirroring() != Cart::MIR_4SCREEN) {
		return prg_ptr_[address & 0x1fff];
	} else {
		return Mapper::read_6(address);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t MMC3::read_7(uint16_t address) {

	if(save_ram_enabled_ && nes::cart.mirroring() != Cart::MIR_4SCREEN) {
		return prg_ptr_[address & 0x1fff];
	} else {
		return Mapper::read_7(address);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void MMC3::write_6(uint16_t address, uint8_t value) {

	if(save_ram_enabled_ && save_ram_writable_ && nes::cart.mirroring() != Cart::MIR_4SCREEN) {
		prg_ptr_[address & 0x1fff] = value;
	} else {
		Mapper::write_6(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void MMC3::write_7(uint16_t address, uint8_t value) {

	if(save_ram_enabled_ && save_ram_writable_ && nes::cart.mirroring() != Cart::MIR_4SCREEN) {
		prg_ptr_[address & 0x1fff] = value;
	} else {
		Mapper::write_7(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void MMC3::write_8(uint16_t address, uint8_t value) {

	switch(address & 0x0001) {
	case 0x0000:
		command_ = value;
		break;
	case 0x0001:
		switch(command_ & 0x07) {
		case 0:
			chr_bank_[0] = (value & 0xfe) | 0x00;
			chr_bank_[1] = (value & 0xfe) | 0x01;
			break;
		case 1:
			chr_bank_[2] = (value & 0xfe) | 0x00;
			chr_bank_[3] = (value & 0xfe) | 0x01;
			break;
		case 2:
			chr_bank_[4] = value;
			break;
		case 3:
			chr_bank_[5] = value;
			break;
		case 4:
			chr_bank_[6] = value;
			break;
		case 5:
			chr_bank_[7] = value;
			break;
		case 6:
			prg_bank_[0] = value & 0x3f;
			break;
		case 7:
			prg_bank_[1] = value & 0x3f;
			break;
		}
		break;
	}

	// do we sync the PRG/CHR on even writes too?
	set_prg_89(prg_bank(0));
	set_prg_ab(prg_bank(1));
	set_prg_cd(prg_bank(2));
	set_prg_ef(prg_bank(3));

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_03ff(chr_bank(0));
		set_chr_0400_07ff(chr_bank(1));
		set_chr_0800_0bff(chr_bank(2));
		set_chr_0c00_0fff(chr_bank(3));
		set_chr_1000_13ff(chr_bank(4));
		set_chr_1400_17ff(chr_bank(5));
		set_chr_1800_1bff(chr_bank(6));
		set_chr_1c00_1fff(chr_bank(7));
	} else {
		set_chr_0000_03ff_ram(chr_ram_, chr_bank(0));
		set_chr_0400_07ff_ram(chr_ram_, chr_bank(1));
		set_chr_0800_0bff_ram(chr_ram_, chr_bank(2));
		set_chr_0c00_0fff_ram(chr_ram_, chr_bank(3));
		set_chr_1000_13ff_ram(chr_ram_, chr_bank(4));
		set_chr_1400_17ff_ram(chr_ram_, chr_bank(5));
		set_chr_1800_1bff_ram(chr_ram_, chr_bank(6));
		set_chr_1c00_1fff_ram(chr_ram_, chr_bank(7));
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void MMC3::write_a(uint16_t address, uint8_t value) {
	switch(address & 0x0001) {
	case 0x0000:
		if(nes::cart.mirroring() != Cart::MIR_4SCREEN) {
			if(value & 0x01) {
				nes::ppu::set_mirroring(nes::ppu::mirror_horizontal);
			} else {
				nes::ppu::set_mirroring(nes::ppu::mirror_vertical);
			}
		}
		break;
	case 0x0001:
		save_ram_enabled_  = ((value & 0x80) != 0);
		save_ram_writable_ = ((value & 0x40) == 0);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void MMC3::write_c(uint16_t address, uint8_t value) {
	switch(address & 0x0001) {
	case 0x0000:
		irq_latch_ = value;
		break;
	case 0x0001:
		irq_counter_ = 0x00;
		irq_reload_ = true;
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void MMC3::write_e(uint16_t address, uint8_t value) {

	(void)value;

	switch(address & 0x0001) {
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
void MMC3::vram_change_hook(uint16_t vram_address) {

	if(vram_address & 0x1000 && !(prev_vram_address_ & 0x1000)) {
		if ((nes::ppu::cycle_count() - prev_ppu_cycle_) >= 16) {
			clock_irq();
		}

		prev_ppu_cycle_ = nes::ppu::cycle_count();
	}

	prev_vram_address_ = vram_address;
}


//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void MMC3A::clock_irq() {

	/*
	 * When the counter is clocked, if it's not zero, it is decremented,
	 * otherwise it is reloaded with the last value written to $C000. *After*
	 * decrementing/reloading, if the counter is zero and IRQ is enabled via
	 * $E001, the IRQ flag is set.
	 */

	if(irq_counter_ == 0x00) {
		irq_counter_ = irq_latch_;
	} else {
		--irq_counter_;
		irq_reload_ = true;
	}

	if(irq_enabled_ && irq_counter_ == 0 && irq_reload_) {
		nes::cpu::irq(nes::cpu::MAPPER_IRQ);
	}

	irq_reload_ = false;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void MMC3B::clock_irq() {

	/*
	 * When the counter is clocked, if it's not zero, it is decremented,
	 * otherwise it is reloaded with the last value written to $C000. *After*
	 * decrementing/reloading, if the counter is zero and IRQ is enabled via
	 * $E001, the IRQ flag is set.
	 */

	if(irq_counter_ == 0x00) {
		irq_counter_ = irq_latch_;
	} else {
		--irq_counter_;
		irq_reload_ = true;
	}

	if(irq_enabled_ && irq_counter_ == 0) {
		nes::cpu::irq(nes::cpu::MAPPER_IRQ);
	}

	irq_reload_ = false;
}
