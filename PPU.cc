
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
const int cpu_alignment       = 0;

enum {
	STATUS_OVERFLOW = 0x20,
	STATUS_SPRITE0  = 0x40,
	STATUS_VBLANK   = 0x80
};

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
	next_ppu_fetch_address_(0),
	background_pattern_table_(0),
	nametable_(0),
	sprite_pattern_table_(0),
	vram_address_(0),
	hpos_(0),
	vpos_(0),
	address_increment_(0),
	color_intensity_(0),
	latch_(0),
	next_attribute_(0),
	next_tile_index_(0),
	register_2000_(0),
	register_2001_(0),
	register_2007_buffer_(0),
	sprite_address_(0),
	sprite_data_index_(0),
	sprite_size_(0),
	status_(0),
	tile_offset_(0),
	sprite_buffer_(0),
	background_clipping_(false),
	background_visible_(false),
	greyscale_(false),
	nmi_on_vblank_(false),
	odd_frame_(false),
	rendering_(false),
	sprite_clipping_(false),
	sprite_init_(false),
	sprite_zero_found_next_(false),
	sprite_zero_found_curr_(false),
	sprites_visible_(false),
	write_latch_(false),
	write_block_(true),
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

#if 0
	std::generate(sprite_ram_, sprite_ram_ + 0x100,  rand);
#endif
	if(reset_type == nes::HARD_RESET) {
		std::fill_n(nametables_, 0x1000, 0);
		std::fill_n(sprite_ram_, 0x0100, 0);

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

	const bool prev_nmi_on_vblank = nmi_on_vblank();

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
	// it should have a 1 OP delay (which we don't emulate yet, cause I'm not sure how to do it)
	if(!prev_nmi_on_vblank && nmi_on_vblank() && (status_ & STATUS_VBLANK)) {
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
	status_ &= ~STATUS_VBLANK;

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
				const uint16_t sprite_line = (vpos_ - 1) - (sprite_ram_[index]);

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
				const uint16_t sprite_line = (vpos_ - 1) - (sprite_ram_[index]);

				// 3a. If the value is in range, set the sprite overflow flag in $2002 and read
				//     the next 3 entries of OAM (incrementing 'm' after each byte and incrementing
				//     'n' when 'm' overflows); if m = 3, increment n
				if(sprite_line < Size) {
					status_ |= STATUS_OVERFLOW;
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
						status_ |= STATUS_SPRITE0;
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

	// Reading one PPU clock before reads it as clear and never sets the flag
	// or generates NMI for that frame.
	if(ppu_cycle_ != (ppu_read_2002_cycle_ + 1)) {
		status_ |= STATUS_VBLANK;
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
// Name: execute_cycle
//------------------------------------------------------------------------------
void PPU::execute_cycle(const scanline_prerender &) {

	// passes when this is 0...
	if(hpos_ == 1) {
		exit_vblank();
	}

	if(screen_enabled()) {
		if(hpos_ < 1) {
			// idle
		} else if(hpos_ < 257) {
			switch(hpos_ & 0x07) {
			case 1: evaluate_sprites_odd();  open_tile_index(); break;
			case 2: evaluate_sprites_even(); read_tile_index(); break;
			case 3: evaluate_sprites_odd();  open_background_attribute(); break;
			case 4: evaluate_sprites_even(); read_background_attribute(); break;
			case 5: evaluate_sprites_odd();  open_background_pattern<pattern_0>(); break;
			case 6: evaluate_sprites_even(); read_background_pattern<pattern_0>(); break;
			case 7: evaluate_sprites_odd();  open_background_pattern<pattern_1>(); break;
			case 0: evaluate_sprites_even(); read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break;
			}

			if(hpos_ == 256) {
				clock_y();
			}
		} else if(hpos_ < 281) {

			if(hpos_ == 257) {
				update_x_scroll();
			}

			switch(hpos_ & 0x07) {
			case 1: update_sprite_registers(); open_tile_index(); break;           // open the bus for nametable fetch (garbage)
			case 2: update_sprite_registers(); read_tile_index(); break;           // fetch the name table byte (garbage)
			case 3: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
			case 4: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
			case 5: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
			case 6: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
			case 7: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
			case 8: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
			}
		} else if(hpos_ < 305) {
			switch(hpos_ & 0x07) {
			case 1: update_vram_address(); update_sprite_registers(); open_tile_index(); break;           // open the bus for nametable fetch (garbage)
			case 2: update_vram_address(); update_sprite_registers(); read_tile_index(); break;           // fetch the name table byte (garbage)
			case 3: update_vram_address(); update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
			case 4: update_vram_address(); update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
			case 5: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
			case 6: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
			case 7: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
			case 0: update_vram_address(); update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
			}
		} else if(hpos_ < 321) {
			switch(hpos_ & 0x07) {
			case 1: update_sprite_registers(); open_tile_index(); break;           // open the bus for nametable fetch (garbage)
			case 2: update_sprite_registers(); read_tile_index(); break;           // fetch the name table byte (garbage)
			case 3: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
			case 4: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
			case 5: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
			case 6: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
			case 7: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
			case 0: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
			}
		} else if(hpos_ < 337) {
			// fetch first 2 tiles of NEXT scanline
			switch(hpos_ & 0x07) {
			case 1: open_tile_index(); break;           // open the bus for nametable fetch
			case 2: read_tile_index(); break;           // fetch the name table byte
			case 3: open_background_attribute(); break; // open the bus for the attribute fetch
			case 4: read_background_attribute(); break; // fetch the attributes
			case 5: open_background_pattern<pattern_0>(); break; // open the bus for pattern A fetch
			case 6: read_background_pattern<pattern_0>(); break; // read 1st pattern byte from 000PTTTTTTTT0YYY
			case 7: open_background_pattern<pattern_1>(); break; // open the bus for pattern B fetch
			case 0: read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break; // read 2nd pattern byte from 000PTTTTTTTT1YYY
			}
		} else {
			switch(hpos_) {
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
}

//------------------------------------------------------------------------------
// Name: execute_cycle
//------------------------------------------------------------------------------
void PPU::execute_cycle(const scanline_render &target) {

	if(!screen_enabled()) {

		if(hpos_ < 1) {
			// idle
		} else if(hpos_ < 257) {
			const uint8_t pixel = select_blank_pixel();
			if(greyscale_) {
				target.buffer[hpos_ - 1] = palette_[pixel] & 0x30;
			} else {
				target.buffer[hpos_ - 1] = palette_[pixel];
			}
		} else {
			// idle
		}
	} else {

		// skip one clock if the first visible line on odd frames
		if(odd_frame_ && vpos_ == 1 && hpos_ == 0) {
			++hpos_;
		}

		if(hpos_ < 1) {
			// idle
		} else if(hpos_ < 257) {
			switch(hpos_ & 0x07) {
			case 1: render_pixel(target.buffer); evaluate_sprites_odd();  open_tile_index(); break;
			case 2: render_pixel(target.buffer); evaluate_sprites_even(); read_tile_index(); break;
			case 3: render_pixel(target.buffer); evaluate_sprites_odd();  open_background_attribute(); break;
			case 4: render_pixel(target.buffer); evaluate_sprites_even(); read_background_attribute(); break;
			case 5: render_pixel(target.buffer); evaluate_sprites_odd();  open_background_pattern<pattern_0>(); break;
			case 6: render_pixel(target.buffer); evaluate_sprites_even(); read_background_pattern<pattern_0>(); break;
			case 7: render_pixel(target.buffer); evaluate_sprites_odd();  open_background_pattern<pattern_1>(); break;
			case 0: render_pixel(target.buffer); evaluate_sprites_even(); read_background_pattern<pattern_1>(); update_shift_registers_render(); clock_x(); break;
			}

			if(hpos_ == 256) {
				clock_y();
			}
		} else if(hpos_ < 321) {

			if(hpos_ == 257) {
				update_x_scroll();
			}

			switch(hpos_ & 0x07) {
			case 1: update_sprite_registers(); open_tile_index(); break;           // open the bus for nametable fetch (garbage)
			case 2: update_sprite_registers(); read_tile_index(); break;           // fetch the name table byte (garbage)
			case 3: update_sprite_registers(); open_background_attribute(); break; // open the bus for the attribute fetch (garbage)
			case 4: update_sprite_registers(); read_background_attribute(); break; // fetch the attributes (garbage)
			case 5: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_0>(); } else { open_sprite_pattern<8, pattern_0>(); } break;
			case 6: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_0>(); } else { read_sprite_pattern<8, pattern_0>(); } break;
			case 7: update_sprite_registers(); if(sprite_size_ == 16) { open_sprite_pattern<16, pattern_1>(); } else { open_sprite_pattern<8, pattern_1>(); } break;
			case 0: update_sprite_registers(); if(sprite_size_ == 16) { read_sprite_pattern<16, pattern_1>(); } else { read_sprite_pattern<8, pattern_1>(); } break;
			}
		} else if(hpos_ < 337) {

			// fetch first 2 tiles of NEXT scanline
			switch(hpos_ & 0x07) {
			case 1: open_tile_index(); break;			// open the bus for nametable fetch
			case 2: read_tile_index(); break;			// fetch the name table byte
			case 3: open_background_attribute(); break; // open the bus for the attribute fetch
			case 4: read_background_attribute(); break; // fetch the attributes
			case 5: open_background_pattern<pattern_0>(); break; // open the bus for pattern A fetch
			case 6: read_background_pattern<pattern_0>(); break; // read 1st pattern byte from 000PTTTTTTTT0YYY
			case 7: open_background_pattern<pattern_1>(); break; // open the bus for pattern B fetch
			case 0: read_background_pattern<pattern_1>(); update_shift_registers_idle(); clock_x(); break; // read 2nd pattern byte from 000PTTTTTTTT1YYY
			}
		} else {
			switch(hpos_) {
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
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_cycle(const scanline_postrender &target) {
	(void)target;
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_cycle(const scanline_vblank &target) {

	(void)target;

	// I know this should be 241 in theory, but we consider the pre-rendering
	// scanline to be #0 for now
	if(vpos_ == 242) {


		switch(hpos_) {
		case 1:
			enter_vblank();
			break;
		case 3:
			if(nmi_on_vblank() && (status_ & STATUS_VBLANK)) {
				nes::cpu.nmi();
			}
			break;
		}
	}
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_scanline(const scanline_vblank &target) {

	// NOTE: MMC3 isn't quite right in "FAST_CPU" mode
	// (but likely good enough for most games)
	// this is because when executing many cycles at a time
	// the CPU can cause writes which manually toggle A12
	// BUT, the PPU cycle count isn't incremented in step
	// so the MMC3 filtering based on PPU cycles doesn't
	// see the manual toggles :-/

	int cycles = 0;
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_, ++ppu_cycle_) {
		execute_cycle(target);
		cycles += clock_cpu();
	}

#ifdef FAST_CPU
	nes::cpu.exec(cycles);
	nes::cart.mapper()->hsync();
#endif
	++vpos_;
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_scanline(const scanline_postrender &target) {

	int cycles = 0;
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_, ++ppu_cycle_) {
		execute_cycle(target);
		cycles += clock_cpu();
	}

#ifdef FAST_CPU
	nes::cpu.exec(cycles);
	nes::cart.mapper()->hsync();
#endif
	++vpos_;
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_scanline(const scanline_prerender &target) {

	int cycles = 0;
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_, ++ppu_cycle_) {
		execute_cycle(target);
		cycles += clock_cpu();
	}

#ifdef FAST_CPU
	nes::cpu.exec(cycles);
	nes::cart.mapper()->hsync();
#endif
	++vpos_;
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
void PPU::execute_scanline(const scanline_render &target) {

	int cycles = 0;
	for(hpos_ = 0; hpos_ < cycles_per_scanline; ++hpos_, ++ppu_cycle_) {
		execute_cycle(target);
		cycles += clock_cpu();
	}

#ifdef FAST_CPU
	nes::cpu.exec(cycles);
	nes::cart.mapper()->hsync();
#endif
	++vpos_;
}

//------------------------------------------------------------------------------
// Name: clock_cpu
// Desc: optionally executes a CPU cycle returns the number of CPU cycles
//       executed
//------------------------------------------------------------------------------
int PPU::clock_cpu() {
	if((ppu_cycle_ % 3) == cpu_alignment) {
#ifndef FAST_CPU
		nes::cpu.exec(1);
#endif
		return 1;
	}
	return 0;
}
