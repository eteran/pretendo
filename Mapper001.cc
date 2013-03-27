
#include "Mapper001.h"
#include "NES.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(1);

namespace {

enum {
	REG_8000_9FFF = 0x00,
	REG_A000_BFFF = 0x01,
	REG_C000_DFFF = 0x02,
	REG_E000_FFFF = 0x03,
};

}

// TODO: how the heck do we support the "256KB PRG ROM bank" of S[OUX]ROM?
// does that mean the chip # or something?

//------------------------------------------------------------------------------
// Name: Mapper1
//------------------------------------------------------------------------------
Mapper1::Mapper1() : cpu_cycles_(0), latch_(0), write_counter_(0), prg_hi_(0x0f), prg_lo_(0x00) {

	memset(prg_ram_, 0, sizeof(prg_ram_));
	memset(chr_ram_, 0, sizeof(chr_ram_));

	set_prg_89ab(prg_lo_);
	set_prg_cdef(prg_hi_);

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}

	regs_[REG_8000_9FFF] = 0;
	regs_[REG_A000_BFFF] = 0;
	regs_[REG_C000_DFFF] = 0;
	regs_[REG_E000_FFFF] = 0;

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
	if(!(regs_[REG_E000_FFFF] & 0x10)) {
		return prg_ram_[address & 0x1fff];
	} else {
		 return Mapper::read_6(address);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper1::read_7(uint16_t address) {
	if(!(regs_[REG_E000_FFFF] & 0x10)) {
		return prg_ram_[address & 0x1fff];
	} else {
		return Mapper::read_7(address);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper1::write_6(uint16_t address, uint8_t value) {
	if(!(regs_[REG_E000_FFFF] & 0x10)) {
		prg_ram_[address & 0x1fff] = value;
	} else {
		Mapper::write_6(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper1::write_7(uint16_t address, uint8_t value) {
	if(!(regs_[REG_E000_FFFF] & 0x10)) {
		prg_ram_[address & 0x1fff] = value;
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
	if(nes::cpu.cycle_count() - cpu_cycles_ > 1) {

		uint8_t bank = (address >> 12) & 0xf;

		if(value & 0x80) {
			// Reset latch_
			latch_                = 0;
			write_counter_        = 5;
			regs_[REG_8000_9FFF] |= 0x0c;
			bank                  = 0;
		} else {
			// Normal write
			value  &= 1;
			latch_ |= (value << write_counter_);
			++write_counter_;
		}

		// every 5th write things are commited
		if(write_counter_ == 5) {
			if(bank > 0) {
				regs_[(bank & 0xf7) >> 1] = (latch_ & 0x1f);
			}

			// reset latches
			latch_         = 0;
			write_counter_ = 0;

			// set mirroring
			switch(regs_[REG_8000_9FFF] & 0x3) {
			case 0:	nes::ppu.set_mirroring(PPU::mirror_single_low);  break;
			case 1:	nes::ppu.set_mirroring(PPU::mirror_single_high); break;
			case 2:	nes::ppu.set_mirroring(PPU::mirror_vertical);    break;
			case 3:	nes::ppu.set_mirroring(PPU::mirror_horizontal);  break;
			}

			// set CHR-ROM
			if(nes::cart.has_chr_rom()) {
				if(regs_[REG_8000_9FFF] & 0x10) {
					set_chr_0000_0fff(regs_[REG_A000_BFFF]);
					set_chr_1000_1fff(regs_[REG_C000_DFFF]);
				} else {
					set_chr_0000_1fff(regs_[REG_A000_BFFF] >> 1);
				}
			}

			// set PRG-ROM
			switch((regs_[REG_8000_9FFF] >> 2) & 0x03) {
			case 0x00:
			case 0x01:
				set_prg_89abcdef((regs_[REG_E000_FFFF] & 0x0f) >> 1);
				break;
			case 0x02:
				set_prg_89ab(0x00);
				set_prg_cdef(regs_[REG_E000_FFFF] & 0x0f);
				break;
			case 0x03:
				set_prg_89ab(regs_[REG_E000_FFFF] & 0x0f);
				set_prg_cdef(0x0f);
				break;
			}
		}
	}

	cpu_cycles_ = nes::cpu.cycle_count();
}
