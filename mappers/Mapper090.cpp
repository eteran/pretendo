
#include "Mapper090.h"
#include "PPU.h"
#include "NES.h"
#include "Cart.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(90)


// TODO: this mapper is very confusing... I have no idea if I am on the right track
//       with its implementation

namespace {

const uint8_t reverse_bits[256] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

}

//------------------------------------------------------------------------------
// Name: Mapper90
//------------------------------------------------------------------------------
Mapper90::Mapper90() {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	set_chr_0000_1fff(0);

	prg_[0] = 0;
	prg_[1] = 0;
	prg_[2] = 0;
	prg_[3] = 0;

	chr_lo_[0] = 0;
	chr_lo_[1] = 0;
	chr_lo_[2] = 0;
	chr_lo_[3] = 0;
	chr_lo_[4] = 0;
	chr_lo_[5] = 0;
	chr_lo_[6] = 0;
	chr_lo_[7] = 0;

	chr_hi_[0] = 0;
	chr_hi_[1] = 0;
	chr_hi_[2] = 0;
	chr_hi_[3] = 0;
	chr_hi_[4] = 0;
	chr_hi_[5] = 0;
	chr_hi_[6] = 0;
	chr_hi_[7] = 0;

}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper90::name() const {
	return "iNES 90";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper90::read_5(uint16_t address) {

	switch(address) {
	case 0x5000:
		// dip switches
		return 0x80;
	case 0x5800:
		return (multiply_1_ * multiply_2_) >> 4;
	case 0x5801:
		return (multiply_1_ * multiply_2_) & 0xff;
	case 0x5802:
		return prg_ram_;
	}

	return Mapper::read_5(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::write_5(uint16_t address, uint8_t value) {
	switch(address) {
	case 0x5800:
		multiply_1_ = value;
		break;
	case 0x5801:
		multiply_2_ = value;
		break;
	case 0x5803:
		prg_ram_ = value;
		break;
	default:
		Mapper::write_5(address, value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::write_8(uint16_t address, uint8_t value) {

	switch(address & 0x07) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		prg_[address & 0x03] = value & 0x7f;
		break;
	}

	sync_prg();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::write_9(uint16_t address, uint8_t value) {

	switch(address & 0x07) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		chr_lo_[address & 0x07] = value;
		break;
	}

	sync_chr();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::write_a(uint16_t address, uint8_t value) {

	switch(address & 0x07) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		chr_hi_[address & 0x07] = value;
		break;
	}

	sync_chr();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::write_b(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}


//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::write_c(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	// TODO: IRQs!
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::write_d(uint16_t address, uint8_t value) {

	switch(address & 0x03) {
	case 0x00:
		bank_control_.raw = (value & ~0x20);
		break;
	case 0x01:
		switch(value & 0x03) {
		case 0x00: set_mirroring(nes::ppu::mirror_vertical);    break;
		case 0x01: set_mirroring(nes::ppu::mirror_horizontal);  break;
		case 0x02: set_mirroring(nes::ppu::mirror_single_low);  break;
		case 0x03: set_mirroring(nes::ppu::mirror_single_high); break;
		}
		break;
	case 0x02:
		// unknown
		break;
	case 0x03:
		chr_control_.raw = value;
		break;
	}

	sync_prg();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::sync_prg() {

	switch(bank_control_.prg) {
	case 0x00:
		set_prg_67((prg_[3] * 4) + 3);
		set_prg_89abcdef(-1);
		break;
	case 0x01:
		set_prg_67((prg_[3] * 2) + 1);
		set_prg_89ab(prg_[1]);
		set_prg_cdef(-1);
		break;
	case 0x02:
		set_prg_67(prg_[3]);
		set_prg_89(prg_[0]);
		set_prg_ab(prg_[1]);
		set_prg_cd(prg_[2]);
		set_prg_ef(-1);
		break;
	case 0x03:
		set_prg_67(reverse_bits[prg_[3]] >> 1);
		set_prg_89(reverse_bits[prg_[0]] >> 1);
		set_prg_ab(reverse_bits[prg_[1]] >> 1);
		set_prg_cd(reverse_bits[prg_[2]] >> 1);
		set_prg_ef(-1);
		break;
	case 0x04:
		set_prg_67((prg_[3] * 4) + 3);
		set_prg_89abcdef(prg_[3]);
		break;
	case 0x05:
		set_prg_67((prg_[3] * 2) + 1);
		set_prg_89ab(prg_[1]);
		set_prg_cdef(prg_[3]);
		break;
	case 0x06:
		set_prg_67(prg_[3]);
		set_prg_89(prg_[0]);
		set_prg_ab(prg_[1]);
		set_prg_cd(prg_[2]);
		set_prg_ef(prg_[3]);
		break;
	case 0x07:
		set_prg_67(reverse_bits[prg_[3]] >> 1);
		set_prg_89(reverse_bits[prg_[0]] >> 1);
		set_prg_ab(reverse_bits[prg_[1]] >> 1);
		set_prg_cd(reverse_bits[prg_[2]] >> 1);
		set_prg_ef(reverse_bits[prg_[3]] >> 1);
		break;
	}



	if(!bank_control_.map_67) {
		unmap_67();
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper90::sync_chr() {

	if(chr_control_.mirror) {
		switch(bank_control_.chr) {
		case 0x00:
			set_chr_0000_1fff((chr_lo_[0] | (chr_hi_[0] << 8)));
			break;
		case 0x01:
			set_chr_0000_0fff((chr_lo_[0] | (chr_hi_[0] << 8)));
			set_chr_1000_1fff((chr_lo_[4] | (chr_hi_[4] << 8)));
			break;
		case 0x02:
			set_chr_0000_07ff((chr_lo_[0] | (chr_hi_[0] << 8)));
			set_chr_0800_0fff((chr_lo_[2] | (chr_hi_[2] << 8)));
			set_chr_1000_17ff((chr_lo_[0] | (chr_hi_[0] << 8)));
			set_chr_1800_1fff((chr_lo_[6] | (chr_hi_[6] << 8)));
			break;
		case 0x03:
			set_chr_0000_03ff((chr_lo_[0] | (chr_hi_[0] << 8)));
			set_chr_0400_07ff((chr_lo_[1] | (chr_hi_[1] << 8)));
			set_chr_0800_0bff((chr_lo_[2] | (chr_hi_[2] << 8)));
			set_chr_0c00_0fff((chr_lo_[3] | (chr_hi_[3] << 8)));
			set_chr_1000_13ff((chr_lo_[0] | (chr_hi_[0] << 8)));
			set_chr_1400_17ff((chr_lo_[1] | (chr_hi_[1] << 8)));
			set_chr_1800_1bff((chr_lo_[6] | (chr_hi_[6] << 8)));
			set_chr_1c00_1fff((chr_lo_[7] | (chr_hi_[7] << 8)));
			break;
		}
	} else {
		switch(bank_control_.chr) {
		case 0x00:
			set_chr_0000_1fff((chr_lo_[0] | (chr_hi_[0] << 8)));
			break;
		case 0x01:
			set_chr_0000_0fff((chr_lo_[0] | (chr_hi_[0] << 8)));
			set_chr_1000_1fff((chr_lo_[4] | (chr_hi_[4] << 8)));
			break;
		case 0x02:
			set_chr_0000_07ff((chr_lo_[0] | (chr_hi_[0] << 8)));
			set_chr_0800_0fff((chr_lo_[2] | (chr_hi_[2] << 8)));
			set_chr_1000_17ff((chr_lo_[4] | (chr_hi_[4] << 8)));
			set_chr_1800_1fff((chr_lo_[6] | (chr_hi_[6] << 8)));
			break;
		case 0x03:
			set_chr_0000_03ff((chr_lo_[0] | (chr_hi_[0] << 8)));
			set_chr_0400_07ff((chr_lo_[1] | (chr_hi_[1] << 8)));
			set_chr_0800_0bff((chr_lo_[2] | (chr_hi_[2] << 8)));
			set_chr_0c00_0fff((chr_lo_[3] | (chr_hi_[3] << 8)));
			set_chr_1000_13ff((chr_lo_[4] | (chr_hi_[4] << 8)));
			set_chr_1400_17ff((chr_lo_[5] | (chr_hi_[5] << 8)));
			set_chr_1800_1bff((chr_lo_[6] | (chr_hi_[6] << 8)));
			set_chr_1c00_1fff((chr_lo_[7] | (chr_hi_[7] << 8)));
			break;
		}
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper90::read_vram(uint16_t address) {

	if(chr_control_.disabled) {
		
		// 256k blocks = 0x40000
		const uint16_t block_offset = (chr_control_.block * 0x40000);
		const uint8_t *const chr_rom = nes::cart.chr();
		const uint32_t chr_mask      = nes::cart.chr_mask();

		switch((address >> 10) & 0x0f) {
		case 0x00: return chr_rom[((block_offset + (0x0 * 0x400)) & chr_mask)];
		case 0x01: return chr_rom[((block_offset + (0x1 * 0x400)) & chr_mask)];
		case 0x02: return chr_rom[((block_offset + (0x2 * 0x400)) & chr_mask)];
		case 0x03: return chr_rom[((block_offset + (0x3 * 0x400)) & chr_mask)];
		case 0x04: return chr_rom[((block_offset + (0x4 * 0x400)) & chr_mask)];
		case 0x05: return chr_rom[((block_offset + (0x5 * 0x400)) & chr_mask)];
		case 0x06: return chr_rom[((block_offset + (0x6 * 0x400)) & chr_mask)];
		case 0x07: return chr_rom[((block_offset + (0x7 * 0x400)) & chr_mask)];
		
		/*
		case 0x08: return chr_rom[((block_offset + (0x8 * 0x400)) & chr_mask)];
		case 0x09: return chr_rom[((block_offset + (0x9 * 0x400)) & chr_mask)];
		case 0x0a: return chr_rom[((block_offset + (0xa * 0x400)) & chr_mask)];
		case 0x0b: return chr_rom[((block_offset + (0xb * 0x400)) & chr_mask)];
		case 0x0c: return chr_rom[((block_offset + (0xc * 0x400)) & chr_mask)];
		case 0x0d: return chr_rom[((block_offset + (0xd * 0x400)) & chr_mask)];
		case 0x0e: return chr_rom[((block_offset + (0xe * 0x400)) & chr_mask)];
		case 0x0f: return chr_rom[((block_offset + (0xf * 0x400)) & chr_mask)];
		*/
		}
	}

	return Mapper::read_vram(address);
}
