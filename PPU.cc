
#include "PPU.h"
#include "NES.h"
#include "Mapper.h"

#include <iostream>
#include <cstring>
#include <algorithm>

#ifdef FAST_CPU
#include "revbits.h"
#endif

//#define SPRITE_ZERO_HACK

namespace {

const int cycles_per_scanline = 341;
const int cpu_alignment       = 1;

enum {
	SPRITE_VFLIP    = 0x80,
	SPRITE_HFLIP    = 0x40,
	SPRITE_PRIORITY = 0x20,
	SPRITE_ZERO     = 0x10, // internal flag
	SPRITE_COLOR    = 0x03
};


struct pattern_0 {
	static const int     index  = 0;
	static const uint8_t offset = 0;
};

struct pattern_1 {
	static const int     index  = 1;
	static const uint8_t offset = 8;
};

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

const uint8_t powerup_palette[32] = {
	0x09, 0x01, 0x00, 0x01, 0x00, 0x02, 0x02, 0x0D, 0x08, 0x10, 0x08, 0x24, 0x00, 0x00, 0x04, 0x2C,
	0x09, 0x01, 0x34, 0x03, 0x00, 0x04, 0x00, 0x14, 0x08, 0x3A, 0x00, 0x02, 0x00, 0x20, 0x2C, 0x08
};

//------------------------------------------------------------------------------
// Name: attribute_bits
//------------------------------------------------------------------------------
uint8_t attribute_bits(uint16_t vram_address, uint8_t attr_byte) {
	return (attr_byte >> (((vram_address & 0x40) >> 4) | (vram_address & 0x02))) & 0x03;
}

//------------------------------------------------------------------------------
// Name: attribute_address
//------------------------------------------------------------------------------
uint16_t attribute_address(uint16_t vram_address) {
	return 0x23c0 | (vram_address & 0x0c00) | ((vram_address >> 4) & 0x38) | ((vram_address >> 2) & 0x07);
}

//------------------------------------------------------------------------------
// Name: tile_address
//------------------------------------------------------------------------------
uint16_t tile_address(uint16_t vram_address) {
	return 0x2000 | (vram_address & 0x0fff);
}

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool PPU::SpriteEntry::vertical_flip() const {
	return sprite_bytes[2] & SPRITE_VFLIP;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool PPU::SpriteEntry::horizontal_flip() const {
	return sprite_bytes[2] & SPRITE_HFLIP;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool PPU::SpriteEntry::is_sprite_zero() const {
	return sprite_bytes[2] & SPRITE_ZERO;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool PPU::SpriteEntry::is_background() const {
	return sprite_bytes[2] & SPRITE_PRIORITY;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::SpriteEntry::palette() const {
	return sprite_bytes[2] & SPRITE_COLOR;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::SpriteEntry::index() const {
	return sprite_bytes[1];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::SpriteEntry::y() const {
	return sprite_bytes[0];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::SpriteEntry::x() const {
	return sprite_bytes[3];
}


//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
PPU::PPU() :
	ppu_cycle_(0),
	ppu_read_2002_cycle_(0),
	register_2000_(0),
	register_2001_(0),
	sprite_data_index_(0),
	show_sprites_(true) {
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
PPU::~PPU() {
}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void PPU::reset(nes::RESET reset_type) {

	if(reset_type == nes::HARD_RESET) {
		std::fill_n(sprite_ram_, 0x100,  0);
		std::fill_n(nametables_, 0x1000, 0);

		for(int i = 0; i < 8; ++i) {
			sprite_data_[i].pattern[0] = 0;
			sprite_data_[i].pattern[1] = 0;
			sprite_data_[i].sprite_bytes[0] = 0xff;
			sprite_data_[i].sprite_bytes[1] = 0xff;
			sprite_data_[i].sprite_bytes[2] = 0xff;
			sprite_data_[i].sprite_bytes[3] = 0xff;
		}

		memcpy(palette_, powerup_palette, sizeof(palette_));
	}

	address_increment_        = 0x01;
	attribute_queue_[0]       = 0;
	attribute_queue_[1]       = 0;
	background_clipping_      = false;
	background_pattern_table_ = 0x0000;
	background_visible_       = false;
	color_intensity_          = 0x00;
	greyscale_                = false;
	hpos_                     = 0;
	latch_                    = 0x00;
	nametable_                = 0x0000;
	next_attribute_           = 0;
	next_pattern_[0]          = 0;
	next_pattern_[1]          = 0;
	next_tile_index_          = 0;
	nmi_on_vblank_            = false;
	odd_frame_                = false;
	pattern_queue_[0]         = 0;
	pattern_queue_[1]         = 0;
	ppu_cycle_                = 0;
	register_2000_            = 0;
	register_2001_            = 0;
	register_2007_buffer_     = 0x00;
	rendering_                = false;
	sprite_address_           = 0x00;
	sprite_buffer_            = 0xff;
	sprite_clipping_          = false;
	sprite_data_index_        = 0;
	sprite_pattern_table_     = 0x0000;
	sprite_size_              = 0x08;
	sprite_zero_found_curr_   = false;
	sprite_zero_found_next_   = false;
	sprites_visible_          = false;
	status_                   = 0x00;
	tile_offset_              = 0x00;
	vpos_                     = 0;
	vram_address_             = 0x0000;
	write_latch_              = false;
	write_block_              = true;

	std::cout << "[PPU::reset] reset complete" << std::endl;
}

//------------------------------------------------------------------------------
// Name: write2000
//------------------------------------------------------------------------------
void PPU::write2000(uint8_t value) {

	latch_ = value;

	if(write_block_) {
		return;
	}

	register_2000_ = value;

	const bool prev_nmi_on_vblank = nmi_on_vblank_;

	nmi_on_vblank_            = (value & 0x80);
	// 0x40 is EXT bus direction
	sprite_size_              = (value & 0x20) ? 16 : 8;
	background_pattern_table_ = (value & 0x10) << 8;
	sprite_pattern_table_     = (value & 0x08) << 9;
	address_increment_        = (value & 0x04) ? 32 : 1;

	// name table address
	// t:0000110000000000=d:00000011
	nametable_ &= 0xf3ff;
	nametable_ |= (value & 0x03) << 10;


	// we can re-trigger an NMI ... though
	// it should have a 1 OP delay (which we don't emulate yet)
	if(!prev_nmi_on_vblank && nmi_on_vblank_ && (status_ & 0x80)) {
		nes::cpu.nmi();
	}
}

//------------------------------------------------------------------------------
// Name: write2001
//------------------------------------------------------------------------------
void PPU::write2001(uint8_t value) {
	latch_ = value;

	if(write_block_) {
		return;
	}

	register_2001_ = value;

	color_intensity_     = (value & 0xe0) >> 5;
	sprites_visible_     = value & 0x10;
	background_visible_  = value & 0x08;
	sprite_clipping_     = value & 0x04;
	background_clipping_ = value & 0x02;
	greyscale_           = value & 0x01;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::write2002(uint8_t value) {
	latch_ = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::write2003(uint8_t value) {
	latch_          = value;
	sprite_address_ = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::write2004(uint8_t value) {
	latch_ = value;

	// sprite_address_ is an 8-bit type, so wrapping is implicit
	sprite_ram_[sprite_address_++] = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::write2005(uint8_t value) {
	latch_ = value;

	if(write_block_) {
		return;
	}

	write_latch_ = !write_latch_;

	if(write_latch_) {
		// 2005 first write:
        // t:0000000000011111=d:11111000
        // x=d:00000111
        nametable_   &= 0x7fe0;
        nametable_   |= (value & 0xf8) >> 3;
        tile_offset_ = value & 0x07;
	} else {
		// 2005 second write:
        // t:0000001111100000=d:11111000
        // t:0111000000000000=d:00000111
        nametable_ &= ~0x73e0;
		nametable_ |= (value & 0xf8) << 2;
        nametable_ |= (value & 0x07) << 12;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::write2006(uint8_t value) {
	latch_ = value;

	if(write_block_) {
		return;
	}

	write_latch_ = !write_latch_;

	if(write_latch_) {
		// 2006 first write:
	    // t:0011111100000000=d:00111111
	    // t:1100000000000000=0
	    nametable_ &= 0x00ff;
	    nametable_ |= (value & 0x3f) << 8;
	} else {
		// 2006 second write:
	    // t:0000000011111111=d:11111111
	    // v=t
	    nametable_    = (nametable_ & 0x7f00) | (value & 0xff);
	    vram_address_ = nametable_;

		nes::cart.mapper()->vram_change_hook(vram_address_);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::write2007(uint8_t value) {
	latch_ = value;

	const uint16_t temp_address = vram_address_ & 0x3fff;

	if(rendering_ && screen_enabled()) {
		if(address_increment_ == 32) {
			clock_y();
		} else {
			clock_x();
		}
	} else {
		vram_address_ += address_increment_;
	}

	nes::cart.mapper()->vram_change_hook(vram_address_);

	// palette write
	if((temp_address & 0x3f00) == 0x3f00) {

		const uint16_t palette_address = temp_address & 0x1f;
		palette_[palette_address] = value & 0x3f;

		if((palette_address & 0x03) == 0x00) {
			// $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C.
			palette_[palette_address ^ 0x10] = value & 0x3f;
		}

	} else if(vram_banks_[temp_address >> 10].writeable()) {
		nes::cart.mapper()->write_vram(temp_address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::read2000() {
	return latch_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::read2001() {
	return latch_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::read2002() {
	// upper 3 bits of status
	// lower 5 bits of garbage (latch)
	const uint8_t ret = (status_ & 0xe0) | (latch_ & 0x1f);

	// reset scroll/write latch
	write_latch_ = false;

	// clear vblank flag
	status_ &= ~0x80;

	ppu_read_2002_cycle_ = ppu_cycle_;

	return ret;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::read2003() {
	return latch_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::read2004() {
	if(sprite_init_) {
		return 0xff;
	}

	if(!rendering_ || !screen_enabled()) {
		switch(sprite_address_ & 0x03) {
		case 0x00: latch_ = sprite_ram_[sprite_address_] & 0xff; break;
		case 0x01: latch_ = sprite_ram_[sprite_address_] & 0xff; break;
		case 0x02: latch_ = sprite_ram_[sprite_address_] & 0xe3; break;
		case 0x03: latch_ = sprite_ram_[sprite_address_] & 0xff; break;
		}

		return latch_;
	}

	return 0x00;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::read2005() {
	return latch_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::read2006() {
	return latch_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t PPU::read2007() {

	if(!write_block_) {
		const uint16_t temp_address = vram_address_ & 0x3fff;

		if(rendering_ && screen_enabled()) {
			if(address_increment_ == 32) {
				clock_y();
			} else {
				clock_x();
			}
		} else {
			vram_address_ += address_increment_;
		}

		nes::cart.mapper()->vram_change_hook(vram_address_);

		latch_ = register_2007_buffer_;
		register_2007_buffer_ = nes::cart.mapper()->read_vram(temp_address);

		if((temp_address & 0x3f00) == 0x3f00) {
			latch_ = palette_[temp_address & 0x1f];
			if(greyscale_) {
				latch_ &= 0x30;
			}
		}
	}

	return latch_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::sprite_dma(uint8_t value) {
	// drain current cycles, then go ahead and do the DMA
	if(nes::cpu.cycle_count() & 1) {
		nes::cpu.burn(514);
	} else {
		nes::cpu.burn(513);
	}

	uint16_t sprite_addr = (value << 8);

	// do the copy with wrapping
	for(int i = 0; i < 256; ++i) {
		write2004(nes::cart.mapper()->read_memory(sprite_addr++));
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::set_mirroring(uint8_t mir) {

	// utilizes the concept of a mirroring control byte
	// each pair of bits represents a table to be mirrored from
	// bits 0/1 are for bank 8/c
	// bits 2/3 are for bank 9/d
	// bits 4/5 are for bank a/e
	// bits 6/7 are for bank b/f

	// so:
	// vertical:   01000100b, or $44
	// horizontal: 01010000b, or $50
	// 4 screen:   11100100b, or $e4
	// single lo:  00000000b, or $00
	// single hi:  01010101b, or $55

	vram_banks_[0x08].set_pointer(&nametables_[(mir << 0xa) & 0x0c00], true);
	vram_banks_[0x09].set_pointer(&nametables_[(mir << 0x8) & 0x0c00], true);
	vram_banks_[0x0a].set_pointer(&nametables_[(mir << 0x6) & 0x0c00], true);
	vram_banks_[0x0b].set_pointer(&nametables_[(mir << 0x4) & 0x0c00], true);

	// [$3000, $4000) mirrors [$2000, $3000)
	vram_banks_[0x0c].set_pointer(&nametables_[(mir << 0xa) & 0x0c00], true);
	vram_banks_[0x0d].set_pointer(&nametables_[(mir << 0x8) & 0x0c00], true);
	vram_banks_[0x0e].set_pointer(&nametables_[(mir << 0x6) & 0x0c00], true);
	vram_banks_[0x0f].set_pointer(&nametables_[(mir << 0x4) & 0x0c00], true);
}

//------------------------------------------------------------------------------
// Name: unset_vram_bank
//------------------------------------------------------------------------------
void PPU::unset_vram_bank(uint8_t bank) {
	vram_banks_[bank] = VRAMBank();
}

//------------------------------------------------------------------------------
// Name: set_vram_bank
//------------------------------------------------------------------------------
void PPU::set_vram_bank(uint8_t bank, uint8_t *p, bool writeable) {
	vram_banks_[bank] = VRAMBank(p, writeable);
}

//------------------------------------------------------------------------------
// Name: start_frame
//------------------------------------------------------------------------------
void PPU::start_frame() {
	vpos_ = 0;
	rendering_ = true;
}

//------------------------------------------------------------------------------
// Name: end_frame
//------------------------------------------------------------------------------
void PPU::end_frame() {
	odd_frame_ = !odd_frame_;
	rendering_ = false;

	nes::cart.mapper()->ppu_end_frame();
}

//------------------------------------------------------------------------------
// Name: write_vram
//------------------------------------------------------------------------------
void PPU::write_vram(uint16_t address, uint8_t value) {

	assert(vram_banks_[(address >> 10) & 0x0f]);
	assert(vram_banks_[(address >> 10) & 0x0f].writeable());

	vram_banks_[(address >> 10) & 0x0f][address & 0x03ff] = value;
}

//------------------------------------------------------------------------------
// Name: read_vram
//------------------------------------------------------------------------------
uint8_t PPU::read_vram(uint16_t address) {

	assert(vram_banks_[(address >> 10) & 0x0f]);
	return vram_banks_[(address >> 10) & 0x0f][address & 0x03ff];
}

//------------------------------------------------------------------------------
// Name: sprite_pattern_address
//------------------------------------------------------------------------------
template <int Size, class Pattern>
uint16_t PPU::sprite_pattern_address(uint8_t index, uint8_t sprite_line) const {

	if(Size == 16) {
		// 8x16. even sprites use $0000, odd $1000
		return ((index & 1) << 12) | ((index & 0xfe) << 4) | Pattern::offset | (sprite_line & 7) | ((sprite_line & 0x08) << 1);
	} else {
		// 8x8
		return sprite_pattern_table_ | (index << 4) | Pattern::offset | sprite_line;
	}
}


//------------------------------------------------------------------------------
// Name: open_sprite_pattern
//------------------------------------------------------------------------------
template <int Size, class Pattern>
void PPU::open_sprite_pattern() {
	SpriteEntry *const sprite_entry = &sprite_data_[((hpos_ - 1) >> 3) & 0x07];

	if(sprite_entry->y() != 0xff) {
		const uint8_t index = sprite_entry->index();
		uint8_t sprite_line;

		// vertical flip
		if(sprite_entry->vertical_flip()) {
			if(Size == 16) {
				sprite_line = sprite_entry->y() ^ 0x0F;
			} else {
				sprite_line = sprite_entry->y() ^ 0x07;
			}
		} else {
			sprite_line = sprite_entry->y();
		}

		// fetch the actual sprite data
		next_ppu_fetch_address_ = sprite_pattern_address<Size, Pattern>(index, sprite_line);
	} else {
		// fetch the actual sprite data (dummy)
		next_ppu_fetch_address_ = sprite_pattern_address<Size, Pattern>(0xff, 0xff);
	}

	nes::cart.mapper()->vram_change_hook(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name: read_sprite_pattern
//------------------------------------------------------------------------------
template <int Size, class Pattern>
void PPU::read_sprite_pattern() {
	SpriteEntry *const sprite_entry = &sprite_data_[((hpos_ - 1) >> 3) & 0x07];

	sprite_entry->pattern[Pattern::index] = nes::cart.mapper()->read_vram(next_ppu_fetch_address_);

	if(sprite_entry->y() != 0xff) {
		// horizontal flip
		if(sprite_entry->horizontal_flip()) {
#ifndef FAST_CPU
			sprite_entry->pattern[Pattern::index] = reverse_bits[sprite_entry->pattern[Pattern::index]];
#else
			sprite_entry->pattern[Pattern::index] = revbits(sprite_entry->pattern[Pattern::index]);
#endif
		}
	}
}

//------------------------------------------------------------------------------
// Name: evaluate_sprites_even
//------------------------------------------------------------------------------
void PPU::evaluate_sprites_even() {
	// write cycle
	if(hpos_ < 64) {
//		uint8_t index = hpos_ >> 1;
//		sprite_data_[(index >> 2) & 0x07].sprite_bytes[index & 0x03] = sprite_buffer_;
//		printf("SETTING: S-OAM[%d][%d] = %02x\n", (index >> 2) & 0x07, index & 0x03, sprite_buffer_);
	} else if(hpos_ == 256) {
		// TODO: do this part incrementally during cycles 0-255 like the real thing
		if(sprite_size_ == 16) {
			evaluate_sprites<16>();
		} else {
			evaluate_sprites<8>();
		}
	}
}

//------------------------------------------------------------------------------
// Name: evaluate_sprites_odd
//------------------------------------------------------------------------------
void PPU::evaluate_sprites_odd() {

	// read cycle
	if(hpos_ < 64) {
		sprite_buffer_ = 0xff;
	} else {
		//sprite_buffer_ = sprite_ram_[sprite_address_++];
	}
}

//------------------------------------------------------------------------------
// Name: evaluate_sprites
//------------------------------------------------------------------------------
template <int Size>
void PPU::evaluate_sprites() {
	sprite_data_index_      = 0;
	sprite_zero_found_next_ = false;
	
#if 1
	for(int i = 0; i < 8; ++i) {
		sprite_data_[i].sprite_bytes[0] = 0xff; // y
		sprite_data_[i].sprite_bytes[1] = 0xff; // index
		sprite_data_[i].sprite_bytes[2] = 0xff; // attributes
		sprite_data_[i].sprite_bytes[3] = 0xff; // x
	}
#endif
	
	const uint8_t start_address = 0x00; // sprite_address_

	uint8_t index = start_address;

	enum State {
		STATE_1,
		STATE_2,
		STATE_3,
		STATE_4
	} state = STATE_1;

	while(state != STATE_4) {
		switch(state) {
		case STATE_1:
			// 1. Starting at n = 0, read a sprite's Y-coordinate (OAM[n][0], copying it to
			//    the next open slot in secondary OAM (unless 8 sprites have been found, in
			//    which case the write is ignored).
			if(sprite_data_index_ < 8) {
				const uint16_t sprite_line = vpos_ - (sprite_ram_[index] + 1);

				// 1a. If Y-coordinate is in range, copy remaining bytes of sprite data
				//     (OAM[n][1] thru OAM[n][3]) into secondary OAM.
				if(sprite_line < Size) {
					sprite_data_[sprite_data_index_].sprite_bytes[0] = sprite_line;                   // y
					sprite_data_[sprite_data_index_].sprite_bytes[1] = sprite_ram_[index + 1]; 	      // index
					sprite_data_[sprite_data_index_].sprite_bytes[2] = sprite_ram_[index + 2] & 0xe3; // attributes
					sprite_data_[sprite_data_index_].sprite_bytes[3] = sprite_ram_[index + 3]; 	      // x

					// note that we found sprite 0
					if(index == start_address) {
						sprite_data_[sprite_data_index_].sprite_bytes[2] |= SPRITE_ZERO;
						sprite_zero_found_next_ = true;
					}

					++sprite_data_index_;
				}
			}
			state = STATE_2;
			break;
		case STATE_2:
			// 2. Increment n
			index += 4;

			// 2a. If n has overflowed back to zero (all 64 sprites evaluated), go to 4
			if((index & 0xfc) == 0x00) {
				state = STATE_4;
				break;
			}

			// 2b. If less than 8 sprites have been found, go to 1
			if(sprite_data_index_ < 8) {
				state = STATE_1;
				break;
			}

			// 2c. If exactly 8 sprites have been found, disable writes to secondary OAM.
			//     This causes sprites in back to drop out.
			if(sprite_data_index_ == 8) {
				state = STATE_3;
				break;
			}

			state = STATE_3;
			break;
		case STATE_3:
			{
				// 3. Starting at m = 0, evaluate OAM[n][m] as a Y-coordinate.
				const uint16_t sprite_line = vpos_ - (sprite_ram_[index] + 1);

				// 3a. If the value is in range, set the sprite overflow flag in $2002 and read
				//     the next 3 entries of OAM (incrementing 'm' after each byte and incrementing
				//     'n' when 'm' overflows); if m = 3, increment n
				if(sprite_line < Size) {
					status_ |= 0x20;
					++index;
				} else {
					// 3b. If the value is not in range, increment n AND m (without carry). If n overflows
					//     to 0, go to 4; otherwise go to 3
					index = (index & 0x03) | (((index & 0xfc) + 4) & 0xfc);
					index = (index & 0xfc) | (((index & 0x03) + 1) & 0x03);
				}
				if((index & 0xfc) == 0x00) {
					state = STATE_4;
				}
			}
			break;
		default:
			abort();
		}
	}
}

//------------------------------------------------------------------------------
// Name: select_blank_pixel
// Note: the screen is *always* disabled when this is called
//------------------------------------------------------------------------------
uint8_t PPU::select_blank_pixel() const {

	assert(!screen_enabled());

	if((vram_address_ & 0x3f00) == 0x3f00) {
		return vram_address_ & 0x1f;
	} else {
		return 0x00;
	}
}

//------------------------------------------------------------------------------
// Name: select_pixel
// Note: the screen is *always* enabled when this is called
//------------------------------------------------------------------------------
uint8_t PPU::select_pixel(uint8_t index) {

	assert(screen_enabled());

	uint8_t pixel;

	// first identify what the BG pixel would be
	if((background_clipping_ || index >= 8) && background_enabled()) {
		switch(tile_offset_) {
		case 0: pixel = ((pattern_queue_[0] & 0x8000) >> 0x0f) | ((pattern_queue_[1] & 0x8000) >> 0x0e) | ((attribute_queue_[0] & 0x8000) >> 0x0d) | ((attribute_queue_[1] & 0x8000) >> 0x0c); break;
		case 1: pixel = ((pattern_queue_[0] & 0x4000) >> 0x0e) | ((pattern_queue_[1] & 0x4000) >> 0x0d) | ((attribute_queue_[0] & 0x4000) >> 0x0c) | ((attribute_queue_[1] & 0x4000) >> 0x0b); break;
		case 2: pixel = ((pattern_queue_[0] & 0x2000) >> 0x0d) | ((pattern_queue_[1] & 0x2000) >> 0x0c) | ((attribute_queue_[0] & 0x2000) >> 0x0b) | ((attribute_queue_[1] & 0x2000) >> 0x0a); break;
		case 3: pixel = ((pattern_queue_[0] & 0x1000) >> 0x0c) | ((pattern_queue_[1] & 0x1000) >> 0x0b) | ((attribute_queue_[0] & 0x1000) >> 0x0a) | ((attribute_queue_[1] & 0x1000) >> 0x09); break;
		case 4: pixel = ((pattern_queue_[0] & 0x0800) >> 0x0b) | ((pattern_queue_[1] & 0x0800) >> 0x0a) | ((attribute_queue_[0] & 0x0800) >> 0x09) | ((attribute_queue_[1] & 0x0800) >> 0x08); break;
		case 5: pixel = ((pattern_queue_[0] & 0x0400) >> 0x0a) | ((pattern_queue_[1] & 0x0400) >> 0x09) | ((attribute_queue_[0] & 0x0400) >> 0x08) | ((attribute_queue_[1] & 0x0400) >> 0x07); break;
		case 6: pixel = ((pattern_queue_[0] & 0x0200) >> 0x09) | ((pattern_queue_[1] & 0x0200) >> 0x08) | ((attribute_queue_[0] & 0x0200) >> 0x07) | ((attribute_queue_[1] & 0x0200) >> 0x06); break;
		case 7: pixel = ((pattern_queue_[0] & 0x0100) >> 0x08) | ((pattern_queue_[1] & 0x0100) >> 0x07) | ((attribute_queue_[0] & 0x0100) >> 0x06) | ((attribute_queue_[1] & 0x0100) >> 0x05); break;
		default:
			abort();
		}
	} else {
		pixel = 0x00;
	}

	// then see if any of the sprites belong..
	if((sprite_clipping_ || index >= 8) && sprites_enabled()) {
		for(const SpriteEntry *p = sprite_data_; p != &sprite_data_[sprite_data_index_]; ++p) {
			const uint16_t x_offset = index - p->x();

			// is this sprite visible on this pixel?
			if(x_offset < 8) {

				const uint8_t p0 = p->pattern[0];
				const uint8_t p1 = p->pattern[1];

				uint8_t sprite_pixel;

				switch(x_offset) {
				case 0: sprite_pixel = ((p0 & 0x80) >> 7) | ((p1 & 0x80) >> 6); break;
				case 1: sprite_pixel = ((p0 & 0x40) >> 6) | ((p1 & 0x40) >> 5); break;
				case 2: sprite_pixel = ((p0 & 0x20) >> 5) | ((p1 & 0x20) >> 4); break;
				case 3: sprite_pixel = ((p0 & 0x10) >> 4) | ((p1 & 0x10) >> 3); break;
				case 4: sprite_pixel = ((p0 & 0x08) >> 3) | ((p1 & 0x08) >> 2); break;
				case 5: sprite_pixel = ((p0 & 0x04) >> 2) | ((p1 & 0x04) >> 1); break;
				case 6: sprite_pixel = ((p0 & 0x02) >> 1) | ((p1 & 0x02) >> 0); break;
				case 7: sprite_pixel = ((p0 & 0x01) >> 0) | ((p1 & 0x01) << 1); break;
				default:
					abort();
				}

				// this pixel is visible..
				if(sprite_pixel & 0x03) {
					// we rendered a sprite0 pixel which collided with a BG pixel
					// NOTE: according to blargg's tests, a collision doesn't seem
					//       possible to occur on the rightmost pixel
				#ifndef SPRITE_ZERO_HACK
					if(p->is_sprite_zero() && (pixel & 0x03) && (index < 255)) {
				#else
					if(p->is_sprite_zero() && (index < 255)) {
				#endif				
						status_ |= 0x40;
						sprite_zero_found_curr_ = false;
					}

					if((!p->is_background() || ((pixel & 0x03) == 0x00)) && show_sprites_) {
						pixel = 0x10 | sprite_pixel | (p->palette() << 2);
					}

					// don't process any more sprites
					break;
				}
			}
		}
	}

	return pixel;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
template <class Pattern>
void PPU::open_background_pattern() {

	const uint8_t tile_line = (vram_address_ & 0x7000) >> 12;
	next_ppu_fetch_address_ = background_pattern_table_ | (next_tile_index_ << 4) | Pattern::offset | tile_line;
	nes::cart.mapper()->vram_change_hook(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
template <class Pattern>
void PPU::read_background_pattern() {

	next_pattern_[Pattern::index] = nes::cart.mapper()->read_vram(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::open_background_attribute() {
	next_ppu_fetch_address_ = attribute_address(vram_address_);
	nes::cart.mapper()->vram_change_hook(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::read_background_attribute() {

	// fetch the attribute byte
	const uint8_t attr_byte = nes::cart.mapper()->read_vram(next_ppu_fetch_address_);

	// get the attribute bits relevant for this tile
	next_attribute_ = attribute_bits(vram_address_, attr_byte);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::open_tile_index() {
	next_ppu_fetch_address_ = tile_address(vram_address_);
	nes::cart.mapper()->vram_change_hook(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::read_tile_index() {
	next_tile_index_ = nes::cart.mapper()->read_vram(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name: clock_x
//------------------------------------------------------------------------------
void PPU::clock_x() {
	// wrap X at 31 and flip bit 10, or just increment
	if((vram_address_ & 0x1f) == 0x1f) {
		vram_address_ ^= 0x41f;
	} else {
		++vram_address_;
	}
}

//------------------------------------------------------------------------------
// Name: clock_y
//------------------------------------------------------------------------------
void PPU::clock_y() {
	// NOTES:
	//    Pertinent VRAM addresses are really 15 bits, as the last one
	//    isn't used.  Bits 14-12 are the tileY offset.  Bits 11-0 hold
	//    the nametable address.
	//
	//    Bits 9-5 represent the y scroll(*8).  It wraps to 0 and bit 11 is
	//    switched when it's incremented from _29_.
	//
	//    Y scroll still wraps from 31, but without flipping bit 11
	if((vram_address_ & 0x7000) == 0x7000) {

		// tile Y wraps from 7 -> 0
		vram_address_ &= 0x0fff;

		// Now Check the Y Scroll
		switch(vram_address_ & 0x03e0) {
		case 0x03a0:
			// Y scroll is 29
			vram_address_ &= ~0x03e0;
			vram_address_ ^= 0x0800;
			break;
		case 0x03e0:
			// Y scroll is 31
			vram_address_ &= ~0x03e0;
			break;
		default:
			// increment the bitfield 9-5
			// NOTE: while it may seem like we need to concern ourselves with
			//       this 5-bit field overflowing into bit 10. There is nothing
			//       to worry about :-). That is handled by the case for $03e0
			//       above.
			vram_address_ += 0x20;
			break;
		}
	} else {
		vram_address_ += 0x1000;
	}
}

//------------------------------------------------------------------------------
// Name: enter_vblank
//------------------------------------------------------------------------------
void PPU::enter_vblank() {

	// Reading 1 PPU clock before VBL should suppress setting
	if(ppu_cycle_ != (ppu_read_2002_cycle_ + 1)) {

		status_ |= 0x80;

		if(nmi_on_vblank()) {
			nes::cpu.nmi();
		}
	}
}

//------------------------------------------------------------------------------
// Name: exit_vblank
//------------------------------------------------------------------------------
void PPU::exit_vblank() {
	// clear all the relevant status bits
	status_ &= 0x1f;
	
	write_block_ = false;
}

//------------------------------------------------------------------------------
// Name: color_intensity
//------------------------------------------------------------------------------
uint8_t PPU::color_intensity() const {
	return color_intensity_;
}

//------------------------------------------------------------------------------
// Name: sprites_enabled
//------------------------------------------------------------------------------
bool PPU::sprites_enabled() const {
	return sprites_visible_;
}

//------------------------------------------------------------------------------
// Name: background_enabled
//------------------------------------------------------------------------------
bool PPU::background_enabled() const {
	return background_visible_;
}

//------------------------------------------------------------------------------
// Name: screen_enabled
//------------------------------------------------------------------------------
bool PPU::screen_enabled() const {
	return sprites_enabled() || background_enabled();
}

//------------------------------------------------------------------------------
// Name: nmi_on_vblank
//------------------------------------------------------------------------------
bool PPU::nmi_on_vblank() const {
	return nmi_on_vblank_;
}

//------------------------------------------------------------------------------
// Name: render_pixel
//------------------------------------------------------------------------------
void PPU::render_pixel(uint8_t *dest_buffer) {

	const uint8_t index = hpos_ - 1;
	const uint8_t pixel = select_pixel(index);

	if(greyscale_) {
		dest_buffer[index] = palette_[pixel & 0x03 ? pixel : 0x00] & 0x30;
	} else {
		dest_buffer[index] = palette_[pixel & 0x03 ? pixel : 0x00];
	}

	pattern_queue_[0]   <<= 1;
	pattern_queue_[1]   <<= 1;
	attribute_queue_[0] <<= 1;
	attribute_queue_[1] <<= 1;
}

//------------------------------------------------------------------------------
// Name: update_shift_registers_idle
//------------------------------------------------------------------------------
void PPU::update_shift_registers_idle() {

	pattern_queue_[0]   <<= 8;
	pattern_queue_[1]   <<= 8;
	attribute_queue_[0] <<= 8;
	attribute_queue_[1] <<= 8;

	pattern_queue_[0]   |= (next_pattern_[0] & 0x00ff);
	pattern_queue_[1]   |= (next_pattern_[1] & 0x00ff);
	attribute_queue_[0] |= (((next_attribute_ >> 0) & 0x01) * 0xff); // we multiply here to "replicate" this bit 8 times (it is used for a whole tile)
	attribute_queue_[1] |= (((next_attribute_ >> 1) & 0x01) * 0xff); // we multiply here to "replicate" this bit 8 times (it is used for a whole tile)
}

//------------------------------------------------------------------------------
// Name: update_shift_registers_render
//------------------------------------------------------------------------------
void PPU::update_shift_registers_render() {

	pattern_queue_[0]   |= (next_pattern_[0] & 0x00ff);
	pattern_queue_[1]   |= (next_pattern_[1] & 0x00ff);
	attribute_queue_[0] |= (((next_attribute_ >> 0) & 0x01) * 0xff); // we multiply here to "replicate" this bit 8 times (it is used for a whole tile)
	attribute_queue_[1] |= (((next_attribute_ >> 1) & 0x01) * 0xff); // we multiply here to "replicate" this bit 8 times (it is used for a whole tile)
}

//------------------------------------------------------------------------------
// Name: update_x_scroll
// Note: occurs at cycle 257 of all rendering scanlines
//------------------------------------------------------------------------------
void PPU::update_x_scroll() {
	// v:0000010000011111=t:0000010000011111
	vram_address_ = (vram_address_ & 0xfbe0) | (nametable_ & 0x041f);
}

//------------------------------------------------------------------------------
// Name: update_sprite_registers
// Note: occurs at cycles 257 - 320 of all rendering scanlines
//------------------------------------------------------------------------------
void PPU::update_sprite_registers() {
	// this gets set to $00 for each tick between 257 and 320
	sprite_address_         = 0;
	sprite_zero_found_curr_ = sprite_zero_found_next_;
}

//------------------------------------------------------------------------------
// Name: update_vram_address
// Note: occurs at cycles 279 - 304 of prerender if screen is enabled
//------------------------------------------------------------------------------
void PPU::update_vram_address() {
	// v=t
	vram_address_ = (vram_address_ & ~0x7be0) | (nametable_ & 0x7be0);
}



//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::execute_cycle(uint8_t *dest_buffer, const scanline_prerender &) {

	(void)dest_buffer;
	
	if(!screen_enabled()) {
		switch(hpos_) {
		case 0:
			// idle
			break;
		case 1:
			exit_vblank();
			break;
		case 2:   case 3:   case 4:   case 5:   case 6:   case 7:
		case 8:   case 9:   case 10:  case 11:  case 12:  case 13:  case 14:  case 15:
		case 16:  case 17:  case 18:  case 19:  case 20:  case 21:  case 22:  case 23:
		case 24:  case 25:  case 26:  case 27:  case 28:  case 29:  case 30:  case 31:
		case 32:  case 33:  case 34:  case 35:  case 36:  case 37:  case 38:  case 39:
		case 40:  case 41:  case 42:  case 43:  case 44:  case 45:  case 46:  case 47:
		case 48:  case 49:  case 50:  case 51:  case 52:  case 53:  case 54:  case 55:
		case 56:  case 57:  case 58:  case 59:  case 60:  case 61:  case 62:  case 63:
		case 64:  case 65:  case 66:  case 67:  case 68:  case 69:  case 70:  case 71:
		case 72:  case 73:  case 74:  case 75:  case 76:  case 77:  case 78:  case 79:
		case 80:  case 81:  case 82:  case 83:  case 84:  case 85:  case 86:  case 87:
		case 88:  case 89:  case 90:  case 91:  case 92:  case 93:  case 94:  case 95:
		case 96:  case 97:  case 98:  case 99:  case 100: case 101: case 102: case 103:
		case 104: case 105: case 106: case 107: case 108: case 109: case 110: case 111:
		case 112: case 113: case 114: case 115: case 116: case 117: case 118: case 119:
		case 120: case 121: case 122: case 123: case 124: case 125: case 126: case 127:
		case 128: case 129: case 130: case 131: case 132: case 133: case 134: case 135:
		case 136: case 137: case 138: case 139: case 140: case 141: case 142: case 143:
		case 144: case 145: case 146: case 147: case 148: case 149: case 150: case 151:
		case 152: case 153: case 154: case 155: case 156: case 157: case 158: case 159:
		case 160: case 161: case 162: case 163: case 164: case 165: case 166: case 167:
		case 168: case 169: case 170: case 171: case 172: case 173: case 174: case 175:
		case 176: case 177: case 178: case 179: case 180: case 181: case 182: case 183:
		case 184: case 185: case 186: case 187: case 188: case 189: case 190: case 191:
		case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199:
		case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207:
		case 208: case 209: case 210: case 211: case 212: case 213: case 214: case 215:
		case 216: case 217: case 218: case 219: case 220: case 221: case 222: case 223:
		case 224: case 225: case 226: case 227: case 228: case 229: case 230: case 231:
		case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239:
		case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247:
		case 248: case 249: case 250: case 251: case 252: case 253: case 254: case 255:
		case 256: case 257: case 258: case 259: case 260: case 261: case 262: case 263:
		case 264: case 265: case 266: case 267: case 268: case 269: case 270: case 271:
		case 272: case 273: case 274: case 275: case 276: case 277: case 278: case 279:
		case 280: case 281: case 282: case 283: case 284: case 285: case 286: case 287:
		case 288: case 289: case 290: case 291: case 292: case 293: case 294: case 295:
		case 296: case 297: case 298: case 299: case 300: case 301: case 302: case 303:
		case 304: case 305: case 306: case 307: case 308: case 309: case 310: case 311:
		case 312: case 313: case 314: case 315: case 316: case 317: case 318: case 319:
		case 320: case 321: case 322: case 323: case 324: case 325: case 326: case 327:
		case 328: case 329: case 330: case 331: case 332: case 333: case 334: case 335:
		case 336: case 337: case 338: case 339: case 340:
			// idle
			break;
		default:
			abort();
			break;
		}
	} else {
		switch(hpos_) {
		case 0:
			// idle
			break;
		
		case   1: exit_vblank(); evaluate_sprites_odd(); open_tile_index(); break;
		case   2:				 evaluate_sprites_even();  read_tile_index(); break;
		case   3:				 evaluate_sprites_odd(); open_background_attribute(); break;
		case   4:				 evaluate_sprites_even();  read_background_attribute(); break;
		case   5:				 evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case   6:				 evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case   7:				 evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case   8:				 evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case   9: evaluate_sprites_odd(); open_tile_index(); break;
		case  10: evaluate_sprites_even();  read_tile_index(); break;
		case  11: evaluate_sprites_odd(); open_background_attribute(); break;
		case  12: evaluate_sprites_even();  read_background_attribute(); break;
		case  13: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  14: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  15: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  16: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  17: evaluate_sprites_odd(); open_tile_index(); break;
		case  18: evaluate_sprites_even();  read_tile_index(); break;
		case  19: evaluate_sprites_odd(); open_background_attribute(); break;
		case  20: evaluate_sprites_even();  read_background_attribute(); break;
		case  21: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  22: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  23: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  24: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  25: evaluate_sprites_odd(); open_tile_index(); break;
		case  26: evaluate_sprites_even();  read_tile_index(); break;
		case  27: evaluate_sprites_odd(); open_background_attribute(); break;
		case  28: evaluate_sprites_even();  read_background_attribute(); break;
		case  29: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  30: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  31: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  32: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  33: evaluate_sprites_odd(); open_tile_index(); break;
		case  34: evaluate_sprites_even();  read_tile_index(); break;
		case  35: evaluate_sprites_odd(); open_background_attribute(); break;
		case  36: evaluate_sprites_even();  read_background_attribute(); break;
		case  37: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  38: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  39: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  40: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  41: evaluate_sprites_odd(); open_tile_index(); break;
		case  42: evaluate_sprites_even();  read_tile_index(); break;
		case  43: evaluate_sprites_odd(); open_background_attribute(); break;
		case  44: evaluate_sprites_even();  read_background_attribute(); break;
		case  45: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  46: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  47: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  48: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  49: evaluate_sprites_odd(); open_tile_index(); break;
		case  50: evaluate_sprites_even();  read_tile_index(); break;
		case  51: evaluate_sprites_odd(); open_background_attribute(); break;
		case  52: evaluate_sprites_even();  read_background_attribute(); break;
		case  53: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  54: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  55: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  56: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  57: evaluate_sprites_odd(); open_tile_index(); break;
		case  58: evaluate_sprites_even();  read_tile_index(); break;
		case  59: evaluate_sprites_odd(); open_background_attribute(); break;
		case  60: evaluate_sprites_even();  read_background_attribute(); break;
		case  61: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  62: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  63: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  64: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  65: evaluate_sprites_odd(); open_tile_index(); break;
		case  66: evaluate_sprites_even();  read_tile_index(); break;
		case  67: evaluate_sprites_odd(); open_background_attribute(); break;
		case  68: evaluate_sprites_even();  read_background_attribute(); break;
		case  69: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  70: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  71: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  72: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  73: evaluate_sprites_odd(); open_tile_index(); break;
		case  74: evaluate_sprites_even();  read_tile_index(); break;
		case  75: evaluate_sprites_odd(); open_background_attribute(); break;
		case  76: evaluate_sprites_even();  read_background_attribute(); break;
		case  77: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  78: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  79: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  80: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  81: evaluate_sprites_odd(); open_tile_index(); break;
		case  82: evaluate_sprites_even();  read_tile_index(); break;
		case  83: evaluate_sprites_odd(); open_background_attribute(); break;
		case  84: evaluate_sprites_even();  read_background_attribute(); break;
		case  85: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  86: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  87: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  88: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  89: evaluate_sprites_odd(); open_tile_index(); break;
		case  90: evaluate_sprites_even();  read_tile_index(); break;
		case  91: evaluate_sprites_odd(); open_background_attribute(); break;
		case  92: evaluate_sprites_even();  read_background_attribute(); break;
		case  93: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  94: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  95: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  96: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case  97: evaluate_sprites_odd(); open_tile_index(); break;
		case  98: evaluate_sprites_even();  read_tile_index(); break;
		case  99: evaluate_sprites_odd(); open_background_attribute(); break;
		case 100: evaluate_sprites_even();  read_background_attribute(); break;
		case 101: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 102: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 103: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 104: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 105: evaluate_sprites_odd(); open_tile_index(); break;
		case 106: evaluate_sprites_even();  read_tile_index(); break;
		case 107: evaluate_sprites_odd(); open_background_attribute(); break;
		case 108: evaluate_sprites_even();  read_background_attribute(); break;
		case 109: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 110: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 111: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 112: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 113: evaluate_sprites_odd(); open_tile_index(); break;
		case 114: evaluate_sprites_even();  read_tile_index(); break;
		case 115: evaluate_sprites_odd(); open_background_attribute(); break;
		case 116: evaluate_sprites_even();  read_background_attribute(); break;
		case 117: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 118: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 119: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 120: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 121: evaluate_sprites_odd(); open_tile_index(); break;
		case 122: evaluate_sprites_even();  read_tile_index(); break;
		case 123: evaluate_sprites_odd(); open_background_attribute(); break;
		case 124: evaluate_sprites_even();  read_background_attribute(); break;
		case 125: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 126: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 127: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 128: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 129: evaluate_sprites_odd(); open_tile_index(); break;
		case 130: evaluate_sprites_even();  read_tile_index(); break;
		case 131: evaluate_sprites_odd(); open_background_attribute(); break;
		case 132: evaluate_sprites_even();  read_background_attribute(); break;
		case 133: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 134: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 135: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 136: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 137: evaluate_sprites_odd(); open_tile_index(); break;
		case 138: evaluate_sprites_even();  read_tile_index(); break;
		case 139: evaluate_sprites_odd(); open_background_attribute(); break;
		case 140: evaluate_sprites_even();  read_background_attribute(); break;
		case 141: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 142: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 143: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 144: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 145: evaluate_sprites_odd(); open_tile_index(); break;
		case 146: evaluate_sprites_even();  read_tile_index(); break;
		case 147: evaluate_sprites_odd(); open_background_attribute(); break;
		case 148: evaluate_sprites_even();  read_background_attribute(); break;
		case 149: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 150: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 151: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 152: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 153: evaluate_sprites_odd(); open_tile_index(); break;
		case 154: evaluate_sprites_even();  read_tile_index(); break;
		case 155: evaluate_sprites_odd(); open_background_attribute(); break;
		case 156: evaluate_sprites_even();  read_background_attribute(); break;
		case 157: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 158: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 159: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 160: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 161: evaluate_sprites_odd(); open_tile_index(); break;
		case 162: evaluate_sprites_even();  read_tile_index(); break;
		case 163: evaluate_sprites_odd(); open_background_attribute(); break;
		case 164: evaluate_sprites_even();  read_background_attribute(); break;
		case 165: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 166: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 167: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 168: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 169: evaluate_sprites_odd(); open_tile_index(); break;
		case 170: evaluate_sprites_even();  read_tile_index(); break;
		case 171: evaluate_sprites_odd(); open_background_attribute(); break;
		case 172: evaluate_sprites_even();  read_background_attribute(); break;
		case 173: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 174: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 175: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 176: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 177: evaluate_sprites_odd(); open_tile_index(); break;
		case 178: evaluate_sprites_even();  read_tile_index(); break;
		case 179: evaluate_sprites_odd(); open_background_attribute(); break;
		case 180: evaluate_sprites_even();  read_background_attribute(); break;
		case 181: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 182: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 183: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 184: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 185: evaluate_sprites_odd(); open_tile_index(); break;
		case 186: evaluate_sprites_even();  read_tile_index(); break;
		case 187: evaluate_sprites_odd(); open_background_attribute(); break;
		case 188: evaluate_sprites_even();  read_background_attribute(); break;
		case 189: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 190: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 191: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 192: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 193: evaluate_sprites_odd(); open_tile_index(); break;
		case 194: evaluate_sprites_even();  read_tile_index(); break;
		case 195: evaluate_sprites_odd(); open_background_attribute(); break;
		case 196: evaluate_sprites_even();  read_background_attribute(); break;
		case 197: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 198: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 199: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 200: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 201: evaluate_sprites_odd(); open_tile_index(); break;
		case 202: evaluate_sprites_even();  read_tile_index(); break;
		case 203: evaluate_sprites_odd(); open_background_attribute(); break;
		case 204: evaluate_sprites_even();  read_background_attribute(); break;
		case 205: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 206: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 207: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 208: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 209: evaluate_sprites_odd(); open_tile_index(); break;
		case 210: evaluate_sprites_even();  read_tile_index(); break;
		case 211: evaluate_sprites_odd(); open_background_attribute(); break;
		case 212: evaluate_sprites_even();  read_background_attribute(); break;
		case 213: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 214: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 215: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 216: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 217: evaluate_sprites_odd(); open_tile_index(); break;
		case 218: evaluate_sprites_even();  read_tile_index(); break;
		case 219: evaluate_sprites_odd(); open_background_attribute(); break;
		case 220: evaluate_sprites_even();  read_background_attribute(); break;
		case 221: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 222: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 223: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 224: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 225: evaluate_sprites_odd(); open_tile_index(); break;
		case 226: evaluate_sprites_even();  read_tile_index(); break;
		case 227: evaluate_sprites_odd(); open_background_attribute(); break;
		case 228: evaluate_sprites_even();  read_background_attribute(); break;
		case 229: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 230: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 231: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 232: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 233: evaluate_sprites_odd(); open_tile_index(); break;
		case 234: evaluate_sprites_even();  read_tile_index(); break;
		case 235: evaluate_sprites_odd(); open_background_attribute(); break;
		case 236: evaluate_sprites_even();  read_background_attribute(); break;
		case 237: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 238: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 239: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 240: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 241: evaluate_sprites_odd(); open_tile_index(); break;
		case 242: evaluate_sprites_even();  read_tile_index(); break;
		case 243: evaluate_sprites_odd(); open_background_attribute(); break;
		case 244: evaluate_sprites_even();  read_background_attribute(); break;
		case 245: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 246: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 247: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 248: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;

		case 249: evaluate_sprites_odd(); open_tile_index(); break;
		case 250: evaluate_sprites_even();  read_tile_index(); break;
		case 251: evaluate_sprites_odd(); open_background_attribute(); break;
		case 252: evaluate_sprites_even();  read_background_attribute(); break;
		case 253: evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 254: evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 255: evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 256: evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); clock_y(); break;

		case 257: update_x_scroll(); update_sprite_registers(); open_tile_index(); break;			// open the bus for nametable fetch (garbage)
		case 258:					 update_sprite_registers(); read_tile_index(); break;			// fetch the name table byte (garbage)
		case 259:					 update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 260:					 update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 261: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 262: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 263: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 264: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 265: update_sprite_registers(); open_tile_index(); break;  	  // open the bus for nametable fetch (garbage)
		case 266: update_sprite_registers(); read_tile_index(); break;  		 // fetch the name table byte (garbage)
		case 267: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 268: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 269: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 270: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 271: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 272: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 273: update_sprite_registers(); open_tile_index(); break;  		 // open the bus for nametable fetch (garbage)
		case 274: update_sprite_registers(); read_tile_index(); break;  		 // fetch the name table byte (garbage)
		case 275: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 276: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 277: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 278: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 279: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 280: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 281: update_vram_address(); update_sprite_registers(); open_tile_index(); break;		 // open the bus for nametable fetch (garbage)
		case 282: update_vram_address(); update_sprite_registers(); read_tile_index(); break;		 // fetch the name table byte (garbage)
		case 283: update_vram_address(); update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 284: update_vram_address(); update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 285: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 286: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 287: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 288: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 289: update_vram_address(); update_sprite_registers(); open_tile_index(); break;		 // open the bus for nametable fetch (garbage)
		case 290: update_vram_address(); update_sprite_registers(); read_tile_index(); break;		 // fetch the name table byte (garbage)
		case 291: update_vram_address(); update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 292: update_vram_address(); update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 293: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 294: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 295: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 296: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 297: update_vram_address(); update_sprite_registers(); open_tile_index(); break;		 // open the bus for nametable fetch (garbage)
		case 298: update_vram_address(); update_sprite_registers(); read_tile_index(); break;		 // fetch the name table byte (garbage)
		case 299: update_vram_address(); update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 300: update_vram_address(); update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 301: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 302: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 303: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 304: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 305: update_sprite_registers(); open_tile_index(); break;  		 // open the bus for nametable fetch (garbage)
		case 306: update_sprite_registers(); read_tile_index(); break;  		 // fetch the name table byte (garbage)
		case 307: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 308: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 309: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 310: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 311: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 312: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 313: update_sprite_registers(); open_tile_index(); break;  		 // open the bus for nametable fetch (garbage)
		case 314: update_sprite_registers(); read_tile_index(); break;  		 // fetch the name table byte (garbage)
		case 315: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 316: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 317: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 318: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 319: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 320: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		// fetch first 2 tiles of NEXT scanline
		case 321: open_tile_index(); break;           // open the bus for nametable fetch
		case 322: read_tile_index(); break;           // fetch the name table byte
		case 323: open_background_attribute(); break; // open the bus for the attribute fetch
		case 324: read_background_attribute(); break; // fetch the attributes		
		case 325: open_background_pattern<pattern_0>(); break; // open the bus for pattern A fetch
		case 326: read_background_pattern<pattern_0>(); break; // read 1st pattern byte from 000PTTTTTTTT0YYY
		case 327: open_background_pattern<pattern_1>(); break; // open the bus for pattern B fetch
		case 328: read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break; // read 2nd pattern byte from 000PTTTTTTTT1YYY
		
		case 329: open_tile_index(); break;           // open the bus for nametable fetch
		case 330: read_tile_index(); break;           // fetch the name table byte
		case 331: open_background_attribute(); break; // open the bus for the attribute fetch
		case 332: read_background_attribute(); break; // fetch the attributes		
		case 333: open_background_pattern<pattern_0>(); break; // open the bus for pattern A fetch
		case 334: read_background_pattern<pattern_0>(); break; // read 1st pattern byte from 000PTTTTTTTT0YYY
		case 335: open_background_pattern<pattern_1>(); break; // open the bus for pattern B fetch
		case 336: read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break; // read 2nd pattern byte from 000PTTTTTTTT1YYY
		
		// dummy fetches
		case 337: open_tile_index(); break;
		case 338: read_tile_index(); break;
		case 339: open_tile_index(); break;
		case 340: read_tile_index(); break;
		case 341:
			// idle
			break;
		default:
			abort();
		}
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PPU::execute_cycle(uint8_t *dest_buffer, const scanline_render &) {

	if(!screen_enabled()) {
		switch(hpos_) {
		case 0:   
			// idle
			break;
		case 1:   case 2:   case 3:   case 4:   case 5:   case 6:   case 7:   case 8:   
		case 9:   case 10:  case 11:  case 12:  case 13:  case 14:  case 15:  case 16:  
		case 17:  case 18:  case 19:  case 20:  case 21:  case 22:  case 23:  case 24:  
		case 25:  case 26:  case 27:  case 28:  case 29:  case 30:  case 31:  case 32: 
		case 33:  case 34:  case 35:  case 36:  case 37:  case 38:  case 39:  case 40:
		case 41:  case 42:  case 43:  case 44:  case 45:  case 46:  case 47:  case 48:
		case 49:  case 50:  case 51:  case 52:  case 53:  case 54:  case 55:  case 56:
		case 57:  case 58:  case 59:  case 60:  case 61:  case 62:  case 63:  case 64:
		case 65:  case 66:  case 67:  case 68:  case 69:  case 70:  case 71:  case 72:
		case 73:  case 74:  case 75:  case 76:  case 77:  case 78:  case 79:  case 80:
		case 81:  case 82:  case 83:  case 84:  case 85:  case 86:  case 87:  case 88:
		case 89:  case 90:  case 91:  case 92:  case 93:  case 94:  case 95:  case 96:
		case 97:  case 98:  case 99:  case 100: case 101: case 102: case 103: case 104:
		case 105: case 106: case 107: case 108: case 109: case 110: case 111: case 112:
		case 113: case 114: case 115: case 116: case 117: case 118: case 119: case 120:
		case 121: case 122: case 123: case 124: case 125: case 126: case 127: case 128:
		case 129: case 130: case 131: case 132: case 133: case 134: case 135: case 136:
		case 137: case 138: case 139: case 140: case 141: case 142: case 143: case 144:
		case 145: case 146: case 147: case 148: case 149: case 150: case 151: case 152:
		case 153: case 154: case 155: case 156: case 157: case 158: case 159: case 160:
		case 161: case 162: case 163: case 164: case 165: case 166: case 167: case 168:
		case 169: case 170: case 171: case 172: case 173: case 174: case 175: case 176:
		case 177: case 178: case 179: case 180: case 181: case 182: case 183: case 184:
		case 185: case 186: case 187: case 188: case 189: case 190: case 191: case 192:
		case 193: case 194: case 195: case 196: case 197: case 198: case 199: case 200:
		case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208:
		case 209: case 210: case 211: case 212: case 213: case 214: case 215: case 216:
		case 217: case 218: case 219: case 220: case 221: case 222: case 223: case 224:
		case 225: case 226: case 227: case 228: case 229: case 230: case 231: case 232:
		case 233: case 234: case 235: case 236: case 237: case 238: case 239: case 240:
		case 241: case 242: case 243: case 244: case 245: case 246: case 247: case 248:
		case 249: case 250: case 251: case 252: case 253: case 254: case 255: case 256:
			{
				const uint8_t pixel = select_blank_pixel();
				if(greyscale_) {
					dest_buffer[hpos_ - 1] = palette_[pixel] & 0x30;
				} else {
					dest_buffer[hpos_ - 1] = palette_[pixel];
				}
			}
			break;
		case 257: case 258: case 259: case 260: case 261: case 262: case 263: case 264:
		case 265: case 266: case 267: case 268: case 269: case 270: case 271: case 272:
		case 273: case 274: case 275: case 276: case 277: case 278: case 279: case 280:
		case 281: case 282: case 283: case 284: case 285: case 286: case 287: case 288:
		case 289: case 290: case 291: case 292: case 293: case 294: case 295: case 296:
		case 297: case 298: case 299: case 300: case 301: case 302: case 303: case 304:
		case 305: case 306: case 307: case 308: case 309: case 310: case 311: case 312:
		case 313: case 314: case 315: case 316: case 317: case 318: case 319: case 320:
		case 321: case 322: case 323: case 324: case 325: case 326: case 327: case 328:
		case 329: case 330: case 331: case 332: case 333: case 334: case 335: case 336:
		case 337: case 338: case 339: case 340:
			// idle
			break;
		default:
			abort();
			break;
		}
	} else {
	
		if(odd_frame_ && vpos_ == 1 && hpos_ == 0) {
			++hpos_;
		}
	
		switch(hpos_) {
		case   0:
			// idle may be skiped
			break;

		case   1: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case   2: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case   3: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case   4: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case   5: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case   6: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case   7: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case   8: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case   9: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  10: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  11: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  12: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  13: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  14: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  15: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  16: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  17: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  18: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  19: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  20: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  21: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  22: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  23: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  24: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  25: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  26: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  27: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  28: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  29: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  30: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  31: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  32: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  33: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  34: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  35: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  36: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  37: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  38: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  39: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  40: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  41: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  42: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  43: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  44: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  45: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  46: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  47: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  48: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  49: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  50: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  51: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  52: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  53: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  54: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  55: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  56: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  57: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  58: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  59: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  60: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  61: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  62: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  63: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  64: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  65: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  66: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  67: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  68: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  69: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  70: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  71: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  72: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  73: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  74: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  75: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  76: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  77: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  78: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  79: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  80: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  81: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  82: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  83: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  84: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  85: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  86: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  87: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  88: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  89: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  90: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  91: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case  92: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case  93: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case  94: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case  95: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case  96: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case  97: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case  98: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case  99: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 100: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 101: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 102: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 103: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 104: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 105: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 106: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 107: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 108: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 109: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 110: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 111: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 112: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 113: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 114: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 115: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 116: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 117: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 118: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 119: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 120: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 121: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 122: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 123: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 124: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 125: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 126: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 127: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 128: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 129: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 130: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 131: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 132: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 133: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 134: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 135: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 136: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 137: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 138: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 139: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 140: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 141: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 142: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 143: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 144: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 145: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 146: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 147: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 148: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 149: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 150: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 151: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 152: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 153: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 154: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 155: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 156: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 157: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 158: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 159: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 160: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 161: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 162: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 163: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 164: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 165: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 166: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 167: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 168: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 169: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 170: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 171: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 172: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 173: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 174: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 175: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 176: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 177: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 178: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 179: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 180: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 181: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 182: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 183: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 184: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 185: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 186: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 187: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 188: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 189: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 190: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 191: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 192: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 193: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 194: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 195: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 196: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 197: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 198: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 199: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 200: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 201: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 202: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 203: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 204: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 205: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 206: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 207: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 208: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 209: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 210: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 211: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 212: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 213: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 214: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 215: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 216: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 217: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 218: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 219: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 220: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 221: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 222: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 223: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 224: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 225: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 226: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 227: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 228: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 229: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 230: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 231: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 232: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 233: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 234: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 235: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 236: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 237: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 238: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 239: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 240: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 241: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 242: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 243: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 244: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 245: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 246: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 247: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 248: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;

		case 249: render_pixel(dest_buffer); evaluate_sprites_odd(); open_tile_index(); break;
		case 250: render_pixel(dest_buffer); evaluate_sprites_even();  read_tile_index(); break;
		case 251: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_attribute(); break;
		case 252: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_attribute(); break;
		case 253: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_0>(); break;
		case 254: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_0>(); break;
		case 255: render_pixel(dest_buffer); evaluate_sprites_odd(); open_background_pattern<pattern_1>(); break;
		case 256: render_pixel(dest_buffer); evaluate_sprites_even();  read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); clock_y(); break;

		case 257: update_x_scroll(); update_sprite_registers(); open_tile_index(); break;			// open the bus for nametable fetch (garbage)
		case 258:					 update_sprite_registers(); read_tile_index(); break;			// fetch the name table byte (garbage)
		case 259:					 update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 260:					 update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 261: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 262: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 263: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 264: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 265: update_sprite_registers(); open_tile_index(); break;  	  // open the bus for nametable fetch (garbage)
		case 266: update_sprite_registers(); read_tile_index(); break;  		 // fetch the name table byte (garbage)
		case 267: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 268: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 269: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 270: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 271: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 272: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 273: update_sprite_registers(); open_tile_index(); break;  		 // open the bus for nametable fetch (garbage)
		case 274: update_sprite_registers(); read_tile_index(); break;  		 // fetch the name table byte (garbage)
		case 275: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 276: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 277: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 278: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 279: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 280: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 281: update_sprite_registers(); open_tile_index(); break;  	  // open the bus for nametable fetch (garbage)
		case 282: update_sprite_registers(); read_tile_index(); break;  	  // fetch the name table byte (garbage)
		case 283: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 284: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 285: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 286: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 287: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 288: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 289: update_sprite_registers(); open_tile_index(); break;  	  // open the bus for nametable fetch (garbage)
		case 290: update_sprite_registers(); read_tile_index(); break;  	  // fetch the name table byte (garbage)
		case 291: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 292: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 293: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 294: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 295: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 296: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 297: update_sprite_registers(); open_tile_index(); break;  	  // open the bus for nametable fetch (garbage)
		case 298: update_sprite_registers(); read_tile_index(); break;  	  // fetch the name table byte (garbage)
		case 299: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 300: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 301: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 302: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 303: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 304: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 305: update_sprite_registers(); open_tile_index(); break;  		 // open the bus for nametable fetch (garbage)
		case 306: update_sprite_registers(); read_tile_index(); break;  		 // fetch the name table byte (garbage)
		case 307: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 308: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 309: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 310: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 311: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 312: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		case 313: update_sprite_registers(); open_tile_index(); break;  		 // open the bus for nametable fetch (garbage)
		case 314: update_sprite_registers(); read_tile_index(); break;  		 // fetch the name table byte (garbage)
		case 315: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
		case 316: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
		case 317: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
		case 318: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
		case 319: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
		case 320: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
		
		// fetch first 2 tiles of NEXT scanline
		case 321: open_tile_index(); break;           // open the bus for nametable fetch
		case 322: read_tile_index(); break;           // fetch the name table byte
		case 323: open_background_attribute(); break; // open the bus for the attribute fetch
		case 324: read_background_attribute(); break; // fetch the attributes		
		case 325: open_background_pattern<pattern_0>(); break; // open the bus for pattern A fetch
		case 326: read_background_pattern<pattern_0>(); break; // read 1st pattern byte from 000PTTTTTTTT0YYY
		case 327: open_background_pattern<pattern_1>(); break; // open the bus for pattern B fetch
		case 328: read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break; // read 2nd pattern byte from 000PTTTTTTTT1YYY
		
		case 329: open_tile_index(); break;           // open the bus for nametable fetch
		case 330: read_tile_index(); break;           // fetch the name table byte
		case 331: open_background_attribute(); break; // open the bus for the attribute fetch
		case 332: read_background_attribute(); break; // fetch the attributes		
		case 333: open_background_pattern<pattern_0>(); break; // open the bus for pattern A fetch
		case 334: read_background_pattern<pattern_0>(); break; // read 1st pattern byte from 000PTTTTTTTT0YYY
		case 335: open_background_pattern<pattern_1>(); break; // open the bus for pattern B fetch
		case 336: read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break; // read 2nd pattern byte from 000PTTTTTTTT1YYY
		
		// dummy fetches
		case 337: open_tile_index(); break;
		case 338: read_tile_index(); break;
		case 339: open_tile_index(); break;
		case 340: read_tile_index(); break;
		default:
			abort();

		}
	}
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_scanline(int line, const scanline_vblank &) {

	// NOTE: MMC3 isn't quite right in "FAST_CPU" mode
	// (but likely good enough for most games)
	// this is because when executing many cycles at a time
	// the CPU can cause writes which manually toggle A12
	// BUT, the PPU cycle count isn't incremented in step
	// so the MMC3 filtering based on PPU cycles doesn't
	// see the manual toggles :-/

#ifdef FAST_CPU
	int cycles = 0;
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_) {

		if(line == 0 && hpos_ == 1) {
			enter_vblank();
		}

		if((ppu_cycle_ % 3) == cpu_alignment) {
			++cycles;
		}
		++ppu_cycle_;
	}
	nes::cpu.exec(cycles);
	nes::cart.mapper()->hsync();
#else
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_) {

		if(line == 0 && hpos_ == 1) {
			enter_vblank();
		}

		if((ppu_cycle_ % 3) == cpu_alignment) {
			nes::cpu.exec(1);
		}
		++ppu_cycle_;
	}
#endif
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_scanline(const scanline_postrender &) {
	// Same issue as scanline_vblank version

#ifdef FAST_CPU
	int cycles = 0;
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_) {
		if((ppu_cycle_ % 3) == cpu_alignment) {
			++cycles;
		}
		++ppu_cycle_;
	}
	nes::cpu.exec(cycles);
	nes::cart.mapper()->hsync();
#else
	for(int i = 0; i < cycles_per_scanline; ++i) {
		if((ppu_cycle_ % 3) == cpu_alignment) {
			nes::cpu.exec(1);
		}
		++ppu_cycle_;
	}
#endif
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_scanline(const scanline_prerender &) {

	static uint8_t dummy[1] = {0};
#ifdef FAST_CPU
	int cycles = 0;
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_) {
		execute_cycle(dummy, scanline_prerender());
		
		if((ppu_cycle_ % 3) == cpu_alignment) {
			++cycles;
		}
		++ppu_cycle_;
	}
	nes::cpu.exec(cycles);
	nes::cart.mapper()->hsync();
#else
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_) {
		execute_cycle(dummy, scanline_prerender());
		
		if((ppu_cycle_ % 3) == cpu_alignment) {
			nes::cpu.exec(1);
		}
		++ppu_cycle_;
	}
#endif
	++vpos_;
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_scanline(uint8_t *dest_buffer, const scanline_render &) {

#ifdef FAST_CPU
	int cycles = 0;
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_) {
		execute_cycle(dest_buffer, scanline_render());
		
		if((ppu_cycle_ % 3) == cpu_alignment) {
			++cycles;
		}
		++ppu_cycle_;
	}
	nes::cpu.exec(cycles);
	nes::cart.mapper()->hsync();
#else
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_) {
		execute_cycle(dest_buffer, scanline_render());
		
		if((ppu_cycle_ % 3) == cpu_alignment) {
			nes::cpu.exec(1);
		}
		++ppu_cycle_;
	}
#endif
	++vpos_;
}
