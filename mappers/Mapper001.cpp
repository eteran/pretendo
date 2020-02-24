
#include "Mapper001.h"
#include "Cart.h"
#include "Nes.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(1)

namespace {

enum {
	Control  = 0x00,
	ChrBank0 = 0x01,
	ChrBank1 = 0x02,
	PrgBank  = 0x03,
};

}

// TODO(eteran): how the heck do we support the "256KB PRG ROM bank" of S[OUX]ROM?
// does that mean the chip # or something?

//------------------------------------------------------------------------------
// Name: Mapper1
//------------------------------------------------------------------------------
Mapper1::Mapper1() {

	prg_ptr_ = open_sram(0x2000);

	set_prg_89ab(0);
	set_prg_cdef(-1);

	if (nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}

	// Reset mapper
	write_handler(0x8000, 0x80);
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper1::name() const {
	return "Nintendo MMC1";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper1::read_6(uint16_t address) {
	if (!(regs_[PrgBank] & 0b10000)) {
		return prg_ptr_[address & 0x1fff];
	}
	return Mapper::read_6(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper1::read_7(uint16_t address) {
	if (!(regs_[PrgBank] & 0b10000)) {
		return prg_ptr_[address & 0x1fff];
	}
	return Mapper::read_7(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper1::write_6(uint16_t address, uint8_t value) {
	if (!(regs_[PrgBank] & 0b10000)) {
		prg_ptr_[address & 0x1fff] = value;
	} else {
		Mapper::write_6(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper1::write_7(uint16_t address, uint8_t value) {
	if (!(regs_[PrgBank] & 0b10000)) {
		prg_ptr_[address & 0x1fff] = value;
	} else {
		Mapper::write_7(address, value);
	}
}

//------------------------------------------------------------------------------
// Name: write_8
//------------------------------------------------------------------------------
void Mapper1::write_8(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_9
//------------------------------------------------------------------------------
void Mapper1::write_9(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void Mapper1::write_a(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Mapper1::write_b(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Mapper1::write_c(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Mapper1::write_d(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Mapper1::write_e(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void Mapper1::write_f(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_handler
//------------------------------------------------------------------------------
void Mapper1::write_handler(uint16_t address, uint8_t value) {

	// ignore writes which are only 1 or 2 cycles appart
	if (nes::cpu::cycle_count() - cpu_cycles_ > 1) {

		uint8_t bank = (address >> 12) & 0xf;

		if (value & 0x80) {
			// Reset latch_
			latch_         = 0;
			write_counter_ = 5;
			regs_[Control] |= 0x0c;
			bank = 0;
		} else {
			// Normal write
			value &= 1;
			latch_ |= (value << write_counter_);
			++write_counter_;
		}

		// every 5th write things are commited
		if (write_counter_ == 5) {
			if (bank > 0) {
				regs_[(bank & 0xf7) >> 1] = (latch_ & 0x1f);
			}

			// reset latches
			latch_         = 0;
			write_counter_ = 0;

			// set mirroring
			switch (regs_[Control] & 0x3) {
			case 0:
				set_mirroring(mirror_single_low);
				break;
			case 1:
				set_mirroring(mirror_single_high);
				break;
			case 2:
				set_mirroring(mirror_vertical);
				break;
			case 3:
				set_mirroring(mirror_horizontal);
				break;
			}

			if (nes::cart.has_chr_rom()) {
				// set CHR-ROM
				if (regs_[Control] & 0x10) {
					set_chr_0000_0fff(regs_[ChrBank0]);
					set_chr_1000_1fff(regs_[ChrBank1]);
				} else {
					set_chr_0000_1fff(regs_[ChrBank0] >> 1);
				}
			} else {
				// NOTE(eteran): this is for SNROM, we may need iNES 2.0 to detect
				// SOROM, SUROM and SXROM

				// set CHR-RAM
				if (regs_[Control] & 0x10) {
					set_chr_0000_0fff_ram(chr_ram_, regs_[ChrBank0] & 1);
					set_chr_1000_1fff_ram(chr_ram_, regs_[ChrBank1] & 1);
				}

				// PRG-RAM enable
				prg_ram_enable0_ = regs_[ChrBank0] & 0x10;
				prg_ram_enable1_ = regs_[ChrBank1] & 0x10;
			}

			// set PRG-ROM
			switch ((regs_[Control] >> 2) & 0x03) {
			case 0x00:
			case 0x01:
				set_prg_89abcdef((regs_[PrgBank] & 0x0f) >> 1);
				break;
			case 0x02:
				set_prg_89ab(0x00);
				set_prg_cdef(regs_[PrgBank] & 0x0f);
				break;
			case 0x03:
				set_prg_89ab(regs_[PrgBank] & 0x0f);
				set_prg_cdef(0x0f);
				break;
			}
		}
	}

	cpu_cycles_ = nes::cpu::cycle_count();
}
