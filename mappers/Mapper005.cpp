
#include "Mapper005.h"
#include "PPU.h"
#include "NES.h"
#include "Cart.h"
#include <cstring>
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(5)

namespace {

enum {
	VSPLIT_ENABLE = 0x80,
	VSPLIT_RIGHT  = 0x40,
	VSPLIT_TILE   = 0x1f
};

}

//------------------------------------------------------------------------------
// Name: Mapper5
//------------------------------------------------------------------------------
Mapper5::Mapper5() {

	set_prg_89(-1);
	set_prg_ab(-1);
	set_prg_cd(-1);
	set_prg_ef(-1);

	memset(exram_,      0x00, sizeof(exram_));
	memset(prg_ram_[0], 0x00, sizeof(prg_ram_[0]));
	memset(prg_ram_[1], 0x00, sizeof(prg_ram_[1]));

	prev_vram_address_[0] = 0xffff;
	prev_vram_address_[1] = 0xffff;

	sp_chr_banks_[0] = 0;
	sp_chr_banks_[1] = 0;
	sp_chr_banks_[2] = 0;
	sp_chr_banks_[3] = 0;
	sp_chr_banks_[4] = 0;
	sp_chr_banks_[5] = 0;
	sp_chr_banks_[6] = 0;
	sp_chr_banks_[7] = 0;

	bg_chr_banks_[0] = 0;
	bg_chr_banks_[1] = 0;
	bg_chr_banks_[2] = 0;
	bg_chr_banks_[3] = 0;
	bg_chr_banks_[4] = 0;
	bg_chr_banks_[5] = 0;
	bg_chr_banks_[6] = 0;
	bg_chr_banks_[7] = 0;

	// entries will be NULL if ROM
	prg_ram_banks_[0x00] = nullptr;
	prg_ram_banks_[0x01] = nullptr;
	prg_ram_banks_[0x02] = nullptr;
	prg_ram_banks_[0x03] = nullptr;
	prg_ram_banks_[0x04] = nullptr;
	prg_ram_banks_[0x05] = nullptr;
	prg_ram_banks_[0x06] = nullptr;
	prg_ram_banks_[0x07] = nullptr;
	prg_ram_banks_[0x08] = nullptr;
	prg_ram_banks_[0x09] = nullptr;
	prg_ram_banks_[0x0a] = nullptr;
	prg_ram_banks_[0x0b] = nullptr;
	prg_ram_banks_[0x0c] = nullptr;
	prg_ram_banks_[0x0d] = nullptr;
	prg_ram_banks_[0x0e] = nullptr;
	prg_ram_banks_[0x0f] = nullptr;
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper5::name() const {
	return "Nintendo MMC5";
}

//------------------------------------------------------------------------------
// Name: write_5
//------------------------------------------------------------------------------
void Mapper5::write_5(uint16_t address, uint8_t value) {

	switch(address) {
	case 0x5100:
		prg_mode_ = value & 0x03;
		break;

	case 0x5101:
		chr_mode_ = value & 0x03;
		break;

	case 0x5102:
		prg_ram_protect1_ = value & 0x03;
		break;

	case 0x5103:
		prg_ram_protect2_ = value & 0x03;
		break;

	case 0x5104:
		exram_mode_ = value & 0x3;
		break;

	case 0x5105:
		mirroring_mode_ = value;
		// we do this here so we can allow regular access to just "pass though" to the PPU
		nes::ppu::set_mirroring(value);
		break;

	case 0x5106:
		fill_mode_tile_ = value;
		break;

	case 0x5107:
		fill_mode_attr_ = value & 0x03;
		break;

	case 0x5113:
		prg_ram_banks_[0x06] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x2000) & 0x7fff) + 0x0000;
		prg_ram_banks_[0x07] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x2000) & 0x7fff) + 0x0000;
		break;

	case 0x5114:
		switch(prg_mode_) {
		case 0x00:
		case 0x01:
		case 0x02:
			break;
		case 0x03:
			if(value & 0x80) {
				set_prg_89(value & 0x7f);
				prg_ram_banks_[0x08] = nullptr;
				prg_ram_banks_[0x09] = nullptr;
			} else {
				prg_ram_banks_[0x08] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x2000) & 0x7fff) + 0x0000;
				prg_ram_banks_[0x09] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x2000) & 0x7fff) + 0x0000;
			}
			break;
		}
		break;

	case 0x5115:
		switch(prg_mode_) {
		case 0x00:
			break;
		case 0x01:
		case 0x02:
			if(value & 0x80) {
				set_prg_89ab((value & 0x7f) >> 1);
				prg_ram_banks_[0x08] = nullptr;
				prg_ram_banks_[0x09] = nullptr;
				prg_ram_banks_[0x0a] = nullptr;
				prg_ram_banks_[0x0b] = nullptr;
			} else {
				prg_ram_banks_[0x08] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x4000) & 0x7fff) + 0x0000;
				prg_ram_banks_[0x09] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x4000) & 0x7fff) + 0x1000;
				prg_ram_banks_[0x0a] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x4000) & 0x7fff) + 0x2000;
				prg_ram_banks_[0x0b] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x4000) & 0x7fff) + 0x3000;
			}
			break;
		case 0x03:
			if(value & 0x80) {
				set_prg_ab(value & 0x7f);
				prg_ram_banks_[0x0a] = nullptr;
				prg_ram_banks_[0x0b] = nullptr;
			} else {
				prg_ram_banks_[0x0a] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x2000) & 0x7fff) + 0x0000;
				prg_ram_banks_[0x0b] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x2000) & 0x7fff) + 0x0000;
			}
			break;
		}
		break;

	case 0x5116:
		switch(prg_mode_) {
		case 0x00:
		case 0x01:
			break;
		case 0x02:
		case 0x03:
			if(value & 0x80) {
				set_prg_cd(value & 0x7f);
				prg_ram_banks_[0x0c] = nullptr;
				prg_ram_banks_[0x0d] = nullptr;
			} else {
				prg_ram_banks_[0x0c] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x2000) & 0x7fff) + 0x0000;
				prg_ram_banks_[0x0d] = prg_ram_[(value >> 2) & 0x01] + (((value & 0x03) * 0x2000) & 0x7fff) + 0x0000;
			}
			break;
		}
		break;


	case 0x5117:
		switch(prg_mode_) {
		case 0x00:
			set_prg_89abcdef((value & 0x7f) >> 2);
			prg_ram_banks_[0x08] = nullptr;
			prg_ram_banks_[0x09] = nullptr;
			prg_ram_banks_[0x0a] = nullptr;
			prg_ram_banks_[0x0b] = nullptr;
			prg_ram_banks_[0x0c] = nullptr;
			prg_ram_banks_[0x0d] = nullptr;
			break;
		case 0x01:
			set_prg_cdef((value & 0x7f) >> 1);
			prg_ram_banks_[0x0c] = nullptr;
			prg_ram_banks_[0x0d] = nullptr;
			break;
		case 0x02:
		case 0x03:
			set_prg_ef(value & 0x7f);
			break;
		}
		break;

	case 0x5120:
	case 0x5121:
	case 0x5122:
	case 0x5123:
	case 0x5124:
	case 0x5125:
	case 0x5126:
	case 0x5127:
		last_chr_write_ = CHR_BANK_A;
		sp_chr_banks_[address & 0x07] = value;
		break;

	case 0x5128:
	case 0x5129:
	case 0x512a:
	case 0x512b:
	case 0x512c: // $512c-$512f are not part of the official spec
	case 0x512d: // but are consistent with what makes sense, perhaps
	case 0x512e: // $5128-$512b are partially decoded?
	case 0x512f:
		last_chr_write_ = CHR_BANK_B;
		bg_chr_banks_[(address & 0x07) ^ 0x00] = value;
		bg_chr_banks_[(address & 0x07) ^ 0x04] = value;
		break;

	case 0x5130:
		bg_char_upper_ = (value << 8);
		break;

	case 0x5200:
		vertical_split_mode_ = value;
		break;

	case 0x5201:
		vertical_split_scroll_ = value;
		break;

	case 0x5202:
		vertical_split_bank_ = value;
		break;

	case 0x5203:
		irq_target_ = value;
		break;

	case 0x5204:
		irq_enabled_ = (value & 0x80);
		break;

	case 0x5205:
		multiplier_1_ = value;
		break;

	case 0x5206:
		multiplier_2_ = value;
		break;

	default:
		if(address >= 0x5c00 && (exram_mode_ & 0x03) == 0x02) {
			exram_[address & 0x03ff] = value;
		}
	}
}

//------------------------------------------------------------------------------
// Name: write_6
//------------------------------------------------------------------------------
void Mapper5::write_6(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_7
//------------------------------------------------------------------------------
void Mapper5::write_7(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper5::write_8(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper5::write_9(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper5::write_a(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper5::write_b(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper5::write_c(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper5::write_d(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper5::write_handler(uint16_t address, uint8_t value) {

	const uint8_t bank = (address >> 12) & 0x0f;

	if(prg_ram_protect1_ == 0x02 && prg_ram_protect2_ == 0x01 && prg_ram_banks_[bank]) {
		prg_ram_banks_[bank][address & 0x0fff] = value;
	} else {
		nes::cpu::write(address, value);
	}
}

//------------------------------------------------------------------------------
// Name: read_5
//------------------------------------------------------------------------------
uint8_t Mapper5::read_5(uint16_t address) {

	uint8_t ret = (address >> 8);

	switch(address) {
	case 0x5204:
		ret = irq_status_.raw;
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		irq_status_.pending = false;
		break;

	case 0x5205:
		do {
			const uint16_t x = multiplier_1_ * multiplier_2_;
			ret = x & 0xff;
		} while(0);
		break;

	case 0x5206:
		do {
			const uint16_t x = multiplier_1_ * multiplier_2_;
			ret = (x >> 8) & 0xff;
		} while(0);
		break;

	default:
		if(address >= 0x5c00) {
			switch(exram_mode_) {
			case 0x00:
			case 0x01:
				break;
			case 0x02:
			case 0x03:
				ret = exram_[address & 0x03ff];
				break;
			}

		}
	}
	return ret;
}

//------------------------------------------------------------------------------
// Name: read_6
//------------------------------------------------------------------------------
uint8_t Mapper5::read_6(uint16_t address) {
	return read_handler(address);
}

//------------------------------------------------------------------------------
// Name: read_7
//------------------------------------------------------------------------------
uint8_t Mapper5::read_7(uint16_t address) {
	return read_handler(address);
}

//------------------------------------------------------------------------------
// Name: read_8
//------------------------------------------------------------------------------
uint8_t Mapper5::read_8(uint16_t address) {
	return read_handler(address);
}

//------------------------------------------------------------------------------
// Name: read_9
//------------------------------------------------------------------------------
uint8_t Mapper5::read_9(uint16_t address) {
	return read_handler(address);
}

//------------------------------------------------------------------------------
// Name: read_a
//------------------------------------------------------------------------------
uint8_t Mapper5::read_a(uint16_t address) {
	return read_handler(address);
}

//------------------------------------------------------------------------------
// Name: read_b
//------------------------------------------------------------------------------
uint8_t Mapper5::read_b(uint16_t address) {
	return read_handler(address);
}

//------------------------------------------------------------------------------
// Name: read_c
//------------------------------------------------------------------------------
uint8_t Mapper5::read_c(uint16_t address) {
	return read_handler(address);
}

//------------------------------------------------------------------------------
// Name: read_d
//------------------------------------------------------------------------------
uint8_t Mapper5::read_d(uint16_t address) {
	return read_handler(address);
}

//------------------------------------------------------------------------------
// Name: read_handler
//------------------------------------------------------------------------------
uint8_t Mapper5::read_handler(uint16_t address) {

	const uint8_t bank = (address >> 12) & 0x0f;

	if(prg_ram_banks_[bank]) {
		return prg_ram_banks_[bank][address & 0x0fff];
	}

	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_vram
//------------------------------------------------------------------------------
uint8_t Mapper5::read_vram(uint16_t address) {


	if(vertical_split_bank_ & VSPLIT_ENABLE) {
		printf("VSPLIT\n");
	}


// VSPLIT_RIGHT  = 0x40,
// VSPLIT_TILE   = 0x1f

	switch((address >> 10) & 0x0f) {
	case 0x08:
	case 0x0c:
		// $2000
		switch(mirroring_mode_ & 0x03) {
		case 0x00: return Mapper::read_vram(address);
		case 0x01: return Mapper::read_vram(address);
		case 0x02: return (exram_mode_ & 0x02) ? 0x00 : exram_[address & 0x03ff];
		case 0x03: return (address & 0x03ff) < 0x03c0 ? fill_mode_tile_ : fill_mode_attr_;
		}
		break;

	case 0x09:
	case 0x0d:
		// $2400
		switch((mirroring_mode_ >> 2) & 0x03) {
		case 0x00: return Mapper::read_vram(address);
		case 0x01: return Mapper::read_vram(address);
		case 0x02: return (exram_mode_ & 0x02) ? 0x00 : exram_[address & 0x03ff];
		case 0x03: return (address & 0x03ff) < 0x03c0 ? fill_mode_tile_ : fill_mode_attr_;
		}
		break;

	case 0x0a:
	case 0x0e:
		// $2800
		switch((mirroring_mode_ >> 4) & 0x03) {
		case 0x00: return Mapper::read_vram(address);
		case 0x01: return Mapper::read_vram(address);
		case 0x02: return (exram_mode_ & 0x02) ? 0x00 : exram_[address & 0x03ff];
		case 0x03: return (address & 0x03ff) < 0x03c0 ? fill_mode_tile_ : fill_mode_attr_;
		}
		break;

	case 0x0b:
	case 0x0f:
		// $2c00
		switch((mirroring_mode_ >> 6) & 0x03) {
		case 0x00: return Mapper::read_vram(address);
		case 0x01: return Mapper::read_vram(address);
		case 0x02: return (exram_mode_ & 0x02) ? 0x00 : exram_[address & 0x03ff];
		case 0x03: return (address & 0x03ff) < 0x03c0 ? fill_mode_tile_ : fill_mode_attr_;
		}
		break;

	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		// CHR-ROM ($0000 - $1fff)
		const uint8_t *chr_selector;

		if(sprite_size_ == 16) {
			// there seems to be 128 fetches of tiles before sprites..
			if(fetch_count_ > 128 && fetch_count_ < 160) {
				chr_selector = sp_chr_banks_;
			} else {
				chr_selector = bg_chr_banks_;
			}
		} else {
			if(last_chr_write_ == CHR_BANK_A) {
				chr_selector = sp_chr_banks_;
			} else {
				chr_selector = bg_chr_banks_;
			}
		}

		const uint8_t *chr_rom_banks[8];
		const uint8_t *const chr_rom = nes::cart.chr();
		const uint32_t chr_mask      = nes::cart.chr_mask();

		switch(chr_mode_ & 0x03) {
		case 0x00: // 8K mode
			chr_rom_banks[0] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x2000) & chr_mask) + 0x0000; // $0000
			chr_rom_banks[1] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x2000) & chr_mask) + 0x0400; // $0400
			chr_rom_banks[2] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x2000) & chr_mask) + 0x0800; // $0800
			chr_rom_banks[3] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x2000) & chr_mask) + 0x0c00; // $0c00
			chr_rom_banks[4] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x2000) & chr_mask) + 0x1000; // $1000
			chr_rom_banks[5] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x2000) & chr_mask) + 0x1400; // $1400
			chr_rom_banks[6] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x2000) & chr_mask) + 0x1800; // $1800
			chr_rom_banks[7] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x2000) & chr_mask) + 0x1c00; // $1c00
			break;
		case 0x01: // 4K mode
			chr_rom_banks[0] = chr_rom + (((chr_selector[3] + bg_char_upper_) * 0x1000) & chr_mask) + 0x0000; // $0000
			chr_rom_banks[1] = chr_rom + (((chr_selector[3] + bg_char_upper_) * 0x1000) & chr_mask) + 0x0400; // $0400
			chr_rom_banks[2] = chr_rom + (((chr_selector[3] + bg_char_upper_) * 0x1000) & chr_mask) + 0x0800; // $0800
			chr_rom_banks[3] = chr_rom + (((chr_selector[3] + bg_char_upper_) * 0x1000) & chr_mask) + 0x0c00; // $0c00
			chr_rom_banks[4] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x1000) & chr_mask) + 0x0000; // $1000
			chr_rom_banks[5] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x1000) & chr_mask) + 0x0400; // $1400
			chr_rom_banks[6] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x1000) & chr_mask) + 0x0800; // $1800
			chr_rom_banks[7] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x1000) & chr_mask) + 0x0c00; // $1c00
			break;
		case 0x02: // 2K mode
			chr_rom_banks[0] = chr_rom + (((chr_selector[1] + bg_char_upper_) * 0x0800) & chr_mask) + 0x0000; // $0000
			chr_rom_banks[1] = chr_rom + (((chr_selector[1] + bg_char_upper_) * 0x0800) & chr_mask) + 0x0400; // $0400
			chr_rom_banks[2] = chr_rom + (((chr_selector[3] + bg_char_upper_) * 0x0800) & chr_mask) + 0x0000; // $0800
			chr_rom_banks[3] = chr_rom + (((chr_selector[3] + bg_char_upper_) * 0x0800) & chr_mask) + 0x0400; // $0c00
			chr_rom_banks[4] = chr_rom + (((chr_selector[5] + bg_char_upper_) * 0x0800) & chr_mask) + 0x0000; // $1000
			chr_rom_banks[5] = chr_rom + (((chr_selector[5] + bg_char_upper_) * 0x0800) & chr_mask) + 0x0400; // $1400
			chr_rom_banks[6] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x0800) & chr_mask) + 0x0000; // $1800
			chr_rom_banks[7] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x0800) & chr_mask) + 0x0400; // $1c00
			break;
		case 0x03: // 1K mode
			chr_rom_banks[0] = chr_rom + (((chr_selector[0] + bg_char_upper_) * 0x0400) & chr_mask) + 0x0000; // $0000
			chr_rom_banks[1] = chr_rom + (((chr_selector[1] + bg_char_upper_) * 0x0400) & chr_mask) + 0x0000; // $0400
			chr_rom_banks[2] = chr_rom + (((chr_selector[2] + bg_char_upper_) * 0x0400) & chr_mask) + 0x0000; // $0800
			chr_rom_banks[3] = chr_rom + (((chr_selector[3] + bg_char_upper_) * 0x0400) & chr_mask) + 0x0000; // $0c00
			chr_rom_banks[4] = chr_rom + (((chr_selector[4] + bg_char_upper_) * 0x0400) & chr_mask) + 0x0000; // $1000
			chr_rom_banks[5] = chr_rom + (((chr_selector[5] + bg_char_upper_) * 0x0400) & chr_mask) + 0x0000; // $1400
			chr_rom_banks[6] = chr_rom + (((chr_selector[6] + bg_char_upper_) * 0x0400) & chr_mask) + 0x0000; // $1800
			chr_rom_banks[7] = chr_rom + (((chr_selector[7] + bg_char_upper_) * 0x0400) & chr_mask) + 0x0000; // $1c00
			break;
		}

		return chr_rom_banks[(address >> 10) & 0x0f][address & 0x03ff];
	}

	return Mapper::read_vram(address);
}

//------------------------------------------------------------------------------
// Name: write_vram
//------------------------------------------------------------------------------
void Mapper5::write_vram(uint16_t address, uint8_t value) {
	switch((address >> 10) & 0x0f) {
	case 0x08:
	case 0x0c:
		// $2000
		switch((mirroring_mode_) & 0x03) {
		case 0x00: Mapper::write_vram(address, value); break;
		case 0x01: Mapper::write_vram(address, value); break;
		case 0x02: if(!(exram_mode_ & 0x02)) exram_[address & 0x03ff] = value; break;
		case 0x03:
			break;
		}
		break;
	case 0x09:
	case 0x0d:
		// $2400
		switch((mirroring_mode_ >> 2) & 0x03) {
		case 0x00: Mapper::write_vram(address, value); break;
		case 0x01: Mapper::write_vram(address, value); break;
		case 0x02: if(!(exram_mode_ & 0x02)) exram_[address & 0x03ff] = value; break;
		case 0x03:
			break;
		}
		break;
	case 0x0a:
	case 0x0e:
		// $2800
		switch((mirroring_mode_ >> 4) & 0x03) {
		case 0x00: Mapper::write_vram(address, value); break;
		case 0x01: Mapper::write_vram(address, value); break;
		case 0x02: if(!(exram_mode_ & 0x02)) exram_[address & 0x03ff] = value; break;
		case 0x03:
			break;
		}
		break;
	case 0x0b:
	case 0x0f:
		// $2c00
		switch((mirroring_mode_ >> 6) & 0x03) {
		case 0x00: Mapper::write_vram(address, value); break;
		case 0x01: Mapper::write_vram(address, value); break;
		case 0x02: if((exram_mode_ & 0x02)) exram_[address & 0x03ff] = value; break;
		case 0x03:
			break;
		}
		break;
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		// CHR-ROM is read-only
		break;
	}

	Mapper::write_vram(address, value);
}

//------------------------------------------------------------------------------
// Name: write_2
//------------------------------------------------------------------------------
void Mapper5::write_2(uint16_t address, uint8_t value) {

	switch(address & 0x07) {
	case 0x00:
		sprite_size_ = (value & 0x20) ? 16 : 8;
		break;
	case 0x01:
		// sprites and background disabled
		if(!(value & 0x18)) {
			irq_status_.in_frame = false;
		}
		break;
	}

	Mapper::write_2(address, value);
}

//------------------------------------------------------------------------------
// Name: write_3
//------------------------------------------------------------------------------
void Mapper5::write_3(uint16_t address, uint8_t value) {

	switch(address & 0x07) {
	case 0x00:
		sprite_size_ = (value & 0x20) ? 16 : 8;
		break;
	case 0x01:
		// sprites and background disabled
		if(!(value & 0x18)) {
			irq_status_.in_frame = false;
		}
		break;
	}

	Mapper::write_3(address, value);
}

//------------------------------------------------------------------------------
// Name: vram_change_hook
//------------------------------------------------------------------------------
void Mapper5::vram_change_hook(uint16_t vram_address) {



	// when this is > 128 (32 * 4), we are fetching sprites, not BG tiles
	++fetch_count_;

	// 3 consecutive reads!
	if(vram_address == prev_vram_address_[0] && vram_address == prev_vram_address_[1]) {
		clock_irq();
		fetch_count_ = 0;
	}

	// shift things down
	prev_vram_address_[1] = prev_vram_address_[0];
	prev_vram_address_[0] = vram_address;
}

//------------------------------------------------------------------------------
// Name: clock_irq
//------------------------------------------------------------------------------
void Mapper5::clock_irq() {

	// if the In Frame signal is clear
	if(!irq_status_.in_frame) {

		// set it, reset the IRQ counter to 0, and clear the IRQ Pending flag
		irq_status_.in_frame = true;
		irq_status_.pending  = false;
		irq_counter_ = 0;
	} else {

		// otherwise, increment the IRQ counter. If it now equals the IRQ scanline ($5203),
		// raise IRQ Pending flag
		if(++irq_counter_ == irq_target_) {
			if(irq_enabled_) {
				nes::cpu::irq(nes::cpu::MAPPER_IRQ);
			}

			irq_status_.pending = true;
		}
	}
}

//------------------------------------------------------------------------------
// Name: clock_irq
//------------------------------------------------------------------------------
void Mapper5::ppu_end_frame() {
	// since we have no idea how MMC5 detects the end of the frame,
	// we use this hook for now
	irq_status_.in_frame = false;
}
