
#include "Ppu.h"
#include "Apu.h"
#include "Cart.h"
#include "Compiler.h"
#include "Cpu.h"
#include "Mapper.h"
#include "Nes.h"

#include <algorithm>
#include <cstring>
#include <iostream>

//#define SPRITE_ZERO_HACK

namespace nes {
namespace ppu {

bool show_sprites  = true;
bool system_paused = false;

namespace {

union Status {
	uint8_t raw;
	BitField<uint8_t, 5> overflow;
	BitField<uint8_t, 6> sprite0;
	BitField<uint8_t, 7> vblank;
};

union Control {
	uint8_t raw;
	BitField<uint8_t, 0, 2> nametable;
	BitField<uint8_t, 2> address_increment;
	BitField<uint8_t, 3> sprite_pattern_table;
	BitField<uint8_t, 4> background_pattern_table;
	BitField<uint8_t, 5> large_sprites;
	BitField<uint8_t, 6> master;
	BitField<uint8_t, 7> nmi_on_vblank;
};

union Mask {
	uint8_t raw;
	BitField<uint8_t, 0> monochrome;
	BitField<uint8_t, 1> background_clipping;
	BitField<uint8_t, 2> sprite_clipping;
	BitField<uint8_t, 3> background_visible;
	BitField<uint8_t, 4> sprites_visible;
	BitField<uint8_t, 5, 3> intensity;

	// meta-fields which don't occupy any space :-)
	BitField<uint8_t, 3, 2> screen_enabled;
};

constexpr auto CyclesPerScanline = 341u;
constexpr auto CpuAlignment      = 0u;

constexpr uint8_t StatusOverflow = 0b00100000;
constexpr uint8_t StatusSprite0  = 0b01000000;
constexpr uint8_t StatusVBlank   = 0b10000000;

constexpr uint8_t OamColor    = 0b00000011;
constexpr uint8_t OamZero     = 0b00010000; // internal flag
constexpr uint8_t OamPriority = 0b00100000;
constexpr uint8_t OamHFlip    = 0b01000000;
constexpr uint8_t OamVFlip    = 0b10000000;

struct pattern_0 {
	static constexpr int index      = 0;
	static constexpr uint8_t offset = 0;
};

struct pattern_1 {
	static constexpr int index      = 1;
	static constexpr uint8_t offset = 8;
};

struct size_8px {
	static constexpr int value     = 8;
	static constexpr int flip_mask = 0b00000111;
};

struct size_16px {
	static constexpr int value     = 16;
	static constexpr int flip_mask = 0b00001111;
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
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff};

const uint8_t powerup_palette[32] = {
	0x09, 0x01, 0x00, 0x01, 0x00, 0x02, 0x02, 0x0D, 0x08, 0x10, 0x08, 0x24, 0x00, 0x00, 0x04, 0x2C,
	0x09, 0x01, 0x34, 0x03, 0x00, 0x04, 0x00, 0x14, 0x08, 0x3A, 0x00, 0x02, 0x00, 0x20, 0x2C, 0x08};

//------------------------------------------------------------------------------
// Name: attribute_bits
//------------------------------------------------------------------------------
constexpr uint8_t attribute_bits(uint16_t vram_address, uint8_t attr_byte) {
	return (attr_byte >> (((vram_address & 0x40) >> 4) | (vram_address & 0x02))) & 0x03;
}

//------------------------------------------------------------------------------
// Name: attribute_address
//------------------------------------------------------------------------------
constexpr uint16_t attribute_address(uint16_t vram_address) {
	return 0x23c0 | (vram_address & 0x0c00) | ((vram_address >> 4) & 0x38) | ((vram_address >> 2) & 0x07);
}

//------------------------------------------------------------------------------
// Name: tile_address
//------------------------------------------------------------------------------
constexpr uint16_t tile_address(uint16_t vram_address) {
	return 0x2000 | (vram_address & 0x0fff);
}

struct SpritePatternData {
	uint8_t x;
	uint8_t y;
	uint8_t index;
	uint8_t attr;
	uint8_t patterns[2];
};

// internal variables
SpritePatternData sprite_patterns_[8];
uint8_t current_sprite_index_ = 0;
uint8_t sprite_data_[32]      = {};
uint8_t sprite_ram_[0x100]    = {};
uint8_t left_most_sprite_x_   = 0xff;
uint8_t sprite_address_       = 0;
uint8_t sprite_data_index_    = 0;

uint8_t palette_[0x20];
uint64_t ppu_cycle_              = 0;
uint64_t ppu_read_2002_cycle_    = 0;
uint16_t next_ppu_fetch_address_ = 0;
uint16_t attribute_queue_[2]     = {};
uint16_t pattern_queue_[2]       = {};
uint16_t nametable_              = 0; // loopy's "t"
uint16_t vram_address_           = 0; // loopy's "v"
uint16_t hpos_                   = 0; // pixel counter
uint16_t vpos_                   = 0; // scanline counter
uint8_t next_pattern_[2]         = {};
uint16_t latch_                  = 0;
uint8_t next_attribute_          = 0;
uint8_t next_tile_index_         = 0;
Control ppu_control_             = {0};
Mask ppu_mask_                   = {0};
uint8_t register_2007_buffer_    = 0;
Status status_                   = {0};
uint8_t tile_offset_             = 0; // loopy's "x"
uint8_t monochrome_mask_         = 0xff;

bool odd_frame_   = false;
bool write_latch_ = false;
bool write_block_ = false;

//------------------------------------------------------------------------------
// Name: sprite_pattern_table
// Desc: returns 0x0000 or 0x1000 depending on bit 3 of ppu_control_
//------------------------------------------------------------------------------
uint16_t sprite_pattern_table() {
	return ppu_control_.sprite_pattern_table ? 0x1000 : 0x0000;
}

//------------------------------------------------------------------------------
// Name: background_pattern_table
// Desc: returns 0x0000 or 0x1000 depending on bit 4 of ppu_control_
//------------------------------------------------------------------------------
uint16_t background_pattern_table() {
	return ppu_control_.background_pattern_table ? 0x1000 : 0x0000;
}

//------------------------------------------------------------------------------
// Name:
// Desc:
//------------------------------------------------------------------------------
template <class Pattern>
constexpr uint16_t sprite_pattern_address(uint8_t index, uint8_t sprite_line, const size_8px &) {
	// 8x8
	return (sprite_pattern_table() | (index << 4) | Pattern::offset | sprite_line) & 0xffff;
}

//------------------------------------------------------------------------------
// Name:
// Desc:
//------------------------------------------------------------------------------
template <class Pattern>
constexpr uint16_t sprite_pattern_address(uint8_t index, uint8_t sprite_line, const size_16px &) {
	// 8x16. even sprites use $0000, odd $1000
	return (((index & 1) << 12) | ((index & 0xfe) << 4) | Pattern::offset | (sprite_line & 7) | ((sprite_line & 0x08) << 1)) & 0xffff;
}

//------------------------------------------------------------------------------
// Name:
// Desc:
//------------------------------------------------------------------------------
template <class Size, class Pattern>
constexpr uint16_t sprite_pattern_address(uint8_t index, uint8_t sprite_line) {
	return sprite_pattern_address<Pattern>(index, sprite_line, Size());
}

//------------------------------------------------------------------------------
// Name: render_blank_pixel
// Note: the screen is *always* disabled when this is called
//------------------------------------------------------------------------------
uint8_t render_blank_pixel() {

	if (UNLIKELY((vram_address_ & 0x3f00) == 0x3f00)) {
		return palette_[vram_address_ & 0x1f] & monochrome_mask_;
	} else {
		return palette_[0x00] & monochrome_mask_;
	}
}

//------------------------------------------------------------------------------
// Name: select_bg_pixel
// Note: the screen is *always* enabled when this is called
//------------------------------------------------------------------------------
uint8_t select_bg_pixel(uint16_t index) {

	// first identify what the BG pixel would be
	if ((ppu_mask_.background_clipping || index >= 8) && ppu_mask_.background_visible) {
		const uint16_t mask = (0x8000 >> tile_offset_);

		return (((pattern_queue_[0] & mask) >> (15 - tile_offset_)) |
				((pattern_queue_[1] & mask) >> (14 - tile_offset_)) |
				((attribute_queue_[0] & mask) >> (13 - tile_offset_)) |
				((attribute_queue_[1] & mask) >> (12 - tile_offset_))) &
			   0xff;

	} else {
		return 0x00;
	}
}

//------------------------------------------------------------------------------
// Name: select_pixel
// Note: the screen is *always* enabled when this is called
//------------------------------------------------------------------------------
uint8_t select_pixel(uint16_t index) {

	// default to displaying the BG pixel
	uint8_t pixel = select_bg_pixel(index);

	// are ANY sprites possibly in range?
	if (left_most_sprite_x_ <= index) {

		// then see if any of the sprites belong..
		if ((ppu_mask_.sprite_clipping || index >= 8) && ppu_mask_.sprites_visible) {

			// this will loop at most 8 times
			for (uint8_t spr = 0; spr != sprite_data_index_; ++spr) {

				const SpritePatternData &sprite = sprite_patterns_[spr];

				const uint16_t x_offset = index - sprite.x;

				// is this sprite visible on this pixel?
				if (x_offset < 8) {

					const uint8_t &p0    = sprite.patterns[0];
					const uint8_t &p1    = sprite.patterns[1];
					const uint16_t shift = 7 - x_offset;

					const uint8_t sprite_pixel =
						((p0 >> shift) & 0x01) | (((p1 >> shift) << 0x01) & 0x02);

					// this pixel is visible..
					if (LIKELY(sprite_pixel & 0x03)) {
						// we rendered a sprite0 pixel which collided with a BG pixel
						// NOTE: according to blargg's tests, a collision doesn't seem
						//       possible to occur on the rightmost pixel
#ifndef SPRITE_ZERO_HACK
						if ((sprite.attr & OamZero) && (index < 255) && (pixel & 0x03)) {
#else
						if ((sprite.attr & OamZero) && (index < 255)) {
#endif
							status_.sprite0 = true;
						}

						if ((((sprite.attr & OamPriority) == 0) || ((pixel & 0x03) == 0x00)) && LIKELY(show_sprites)) {
							pixel = (0x10 | sprite_pixel | ((sprite.attr & OamColor) << 2)) & 0xff;
						}

						return pixel;
					}
				}
			}
		}
	}

	return pixel;
}

//------------------------------------------------------------------------------
// Name: clock_x
//------------------------------------------------------------------------------
void clock_x() {
	// wrap X at 31 and flip bit 10, or just increment
	if (UNLIKELY((vram_address_ & 0x1f) == 0x1f)) {
		vram_address_ ^= 0x41f;
	} else {
		++vram_address_;
	}
}

//------------------------------------------------------------------------------
// Name: clock_y
//------------------------------------------------------------------------------
void clock_y() {
	// NOTES:
	//    Pertinent VRAM addresses are really 15 bits, as the last one
	//    isn't used.  Bits 14-12 are the tileY offset.  Bits 11-0 hold
	//    the nametable address.
	//
	//    Bits 9-5 represent the y scroll(*8).  It wraps to 0 and bit 11 is
	//    switched when it's incremented from _29_.
	//
	//    Y scroll still wraps from 31, but without flipping bit 11
	if (UNLIKELY((vram_address_ & 0x7000) == 0x7000)) {

		// tile Y wraps from 7 -> 0
		vram_address_ &= 0x0fff;

		// Now Check the Y Scroll
		switch (vram_address_ & 0x03e0) {
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
// Name:
//------------------------------------------------------------------------------
template <class Pattern>
void open_background_pattern() {

	const uint8_t tile_line = (vram_address_ & 0x7000) >> 12;
	next_ppu_fetch_address_ = (background_pattern_table() | (next_tile_index_ << 4) | Pattern::offset | tile_line) & 0xffff;
	cart.mapper()->vram_change_hook(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
template <class Pattern>
void read_background_pattern() {

	next_pattern_[Pattern::index] = cart.mapper()->read_vram(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void open_background_attribute() {
	next_ppu_fetch_address_ = attribute_address(vram_address_);
	cart.mapper()->vram_change_hook(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void read_background_attribute() {

	// fetch the attribute byte
	const uint8_t attr_byte = cart.mapper()->read_vram(next_ppu_fetch_address_);

	// get the attribute bits relevant for this tile
	next_attribute_ = attribute_bits(vram_address_, attr_byte);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void open_tile_index() {
	next_ppu_fetch_address_ = tile_address(vram_address_);
	cart.mapper()->vram_change_hook(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void read_tile_index() {
	next_tile_index_ = cart.mapper()->read_vram(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool sprite_in_range(uint8_t y) {
	const uint16_t sprite_line = (vpos_ - 1) - y;

    if (ppu_control_.large_sprites) {
        return sprite_line < 16;
    } else {
        return sprite_line < 8;
    }
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t &sprite_y(uint8_t index) {
	return sprite_data_[index * 4 + 0];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t &sprite_index(uint8_t index) {
	return sprite_data_[index * 4 + 1];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t &sprite_attr(uint8_t index) {
	return sprite_data_[index * 4 + 2];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t &sprite_x(uint8_t index) {
	return sprite_data_[index * 4 + 3];
}

//------------------------------------------------------------------------------
// Name: evaluate_sprites
//------------------------------------------------------------------------------
void evaluate_sprites() {
	sprite_data_index_ = 0;

	constexpr uint8_t start_address = 0x00; // sprite_address_

	uint8_t index = start_address;

	enum State {
		STATE_1,
		STATE_2,
		STATE_3,
		STATE_4
	} state = STATE_1;

	while (state != STATE_4) {
		switch (state) {
		case STATE_1:
			// 1. Starting at n = 0, read a sprite's Y-coordinate (OAM[n][0], copying it to
			//    the next open slot in secondary OAM (unless 8 sprites have been found, in
			//    which case the write is ignored).
			if (sprite_data_index_ < 8) {

				// 1a. If Y-coordinate is in range, copy remaining bytes of sprite data
				//     (OAM[n][1] thru OAM[n][3]) into secondary OAM.
                if (sprite_in_range(sprite_ram_[index])) {

					const uint16_t sprite_line       = (vpos_ - 1) - sprite_ram_[index + 0];
					sprite_y(sprite_data_index_)     = static_cast<uint8_t>(sprite_line); // y
					sprite_index(sprite_data_index_) = sprite_ram_[index + 1];            // index
					sprite_attr(sprite_data_index_)  = sprite_ram_[index + 2] & 0xe3;     // attributes
					sprite_x(sprite_data_index_)     = sprite_ram_[index + 3];            // x

					// note that we found sprite 0
					if (index == start_address) {
						sprite_attr(sprite_data_index_) |= OamZero;
					}

					left_most_sprite_x_ = std::min(left_most_sprite_x_, sprite_x(sprite_data_index_));

					++sprite_data_index_;
				}
			}
			state = STATE_2;
			break;
		case STATE_2:
			// 2. Increment n
			index += 4;

			// 2a. If n has overflowed back to zero (all 64 sprites evaluated), go to 4
			if ((index & 0xfc) == 0x00) {
				state = STATE_4;
				break;
			}

			// 2b. If less than 8 sprites have been found, go to 1
			if (sprite_data_index_ < 8) {
				state = STATE_1;
				break;
			}

			// 2c. If exactly 8 sprites have been found, disable writes to secondary OAM.
			//     This causes sprites in back to drop out.
			if (sprite_data_index_ == 8) {
				state = STATE_3;
				break;
			}

			state = STATE_3;
			break;
		case STATE_3: {
			// 3. Starting at m = 0, evaluate OAM[n][m] as a Y-coordinate.
			// 3a. If the value is in range, set the sprite overflow flag in $2002 and read
			//     the next 3 entries of OAM (incrementing 'm' after each byte and incrementing
			//     'n' when 'm' overflows); if m = 3, increment n
            if (sprite_in_range(sprite_ram_[index])) {
				status_.overflow = true;
				++index;
			} else {
				// 3b. If the value is not in range, increment n AND m (without carry). If n overflows
				//     to 0, go to 4; otherwise go to 3
				index = (index & 0x03) | (((index & 0xfc) + 4) & 0xfc);
				index = (index & 0xfc) | (((index & 0x03) + 1) & 0x03);
			}
			if ((index & 0xfc) == 0x00) {
				state = STATE_4;
			}
		} break;
		default:
			abort();
		}
	}
}

//------------------------------------------------------------------------------
// Name: evaluate_sprites_even
//------------------------------------------------------------------------------
void evaluate_sprites_even() {
	// write cycle
	if (hpos_ < 64) {
		if (UNLIKELY(hpos_ == 0)) {
			std::fill_n(sprite_data_, sizeof(sprite_data_), 0xff);
		}

	} else if (UNLIKELY(hpos_ == 256)) {
		// TODO: do this part incrementally during cycles 0-255 like the real thing
        evaluate_sprites();
	}
}

//------------------------------------------------------------------------------
// Name: evaluate_sprites_odd
//------------------------------------------------------------------------------
void evaluate_sprites_odd() {
}

//------------------------------------------------------------------------------
// Name: enter_vblank
//------------------------------------------------------------------------------
void enter_vblank() {

	// Reading one PPU clock before reads it as clear and never sets the flag
	// or generates NMI for that frame.
	if (UNLIKELY(ppu_cycle_ != (ppu_read_2002_cycle_ + 1))) {
		status_.vblank = true;
	}
}

//------------------------------------------------------------------------------
// Name: open_sprite_pattern
//------------------------------------------------------------------------------
template <class Size, class Pattern>
void open_sprite_pattern() {

	current_sprite_index_     = ((hpos_ - 1) >> 3) & 0x07;
	SpritePatternData &sprite = sprite_patterns_[current_sprite_index_];

	sprite.y = sprite_y(current_sprite_index_);

	if (sprite.y != 0xff) {

		sprite.x     = sprite_x(current_sprite_index_);
		sprite.attr  = sprite_attr(current_sprite_index_);
		sprite.index = sprite_index(current_sprite_index_);

		// vertical flip
		if (sprite.attr & OamVFlip) {
			sprite.y ^= Size::flip_mask;
		}

		// fetch the actual sprite data
		next_ppu_fetch_address_ = sprite_pattern_address<Size, Pattern>(sprite.index, sprite.y);
	} else {
		// fetch the actual sprite data (dummy)
		next_ppu_fetch_address_ = sprite_pattern_address<Size, Pattern>(0xff, 0xff);
	}

	cart.mapper()->vram_change_hook(next_ppu_fetch_address_);
}

//------------------------------------------------------------------------------
// Name: read_sprite_pattern
//------------------------------------------------------------------------------
template <class Size, class Pattern>
void read_sprite_pattern() {

	uint8_t pattern = cart.mapper()->read_vram(next_ppu_fetch_address_);

	SpritePatternData &sprite = sprite_patterns_[current_sprite_index_];

	// horizontal flip
	if (sprite.attr & OamHFlip) {
		pattern = reverse_bits[pattern];
	}

	sprite_patterns_[current_sprite_index_].patterns[Pattern::index] = pattern;
}

//------------------------------------------------------------------------------
// Name: render_pixel
//------------------------------------------------------------------------------
uint8_t render_pixel() {

	const uint8_t pixel = select_pixel(hpos_ - 1);

	pattern_queue_[0] <<= 1;
	pattern_queue_[1] <<= 1;
	attribute_queue_[0] <<= 1;
	attribute_queue_[1] <<= 1;

	return palette_[(pixel & 0x03) ? pixel : 0x00] & monochrome_mask_;
}

//------------------------------------------------------------------------------
// Name: update_shift_registers_render
//------------------------------------------------------------------------------
void update_shift_registers_render() {

	pattern_queue_[0] |= next_pattern_[0];
	pattern_queue_[1] |= next_pattern_[1];
	attribute_queue_[0] |= ((next_attribute_ >> 0) & 0x01) * 0xff; // we multiply here to "replicate" this bit 8 times (it is used for a whole tile)
	attribute_queue_[1] |= ((next_attribute_ >> 1) & 0x01) * 0xff; // we multiply here to "replicate" this bit 8 times (it is used for a whole tile)
}

//------------------------------------------------------------------------------
// Name: update_shift_registers_idle
//------------------------------------------------------------------------------
void update_shift_registers_idle() {

	pattern_queue_[0] <<= 8;
	pattern_queue_[1] <<= 8;
	attribute_queue_[0] <<= 8;
	attribute_queue_[1] <<= 8;

	update_shift_registers_render();
}

//------------------------------------------------------------------------------
// Name: update_x_scroll
// Note: occurs at cycle 257 of all rendering scanlines
//------------------------------------------------------------------------------
void update_x_scroll() {
	// v:0000010000011111=t:0000010000011111
	vram_address_ = (vram_address_ & ~0b0000010000011111) | (nametable_ & 0b0000010000011111);
}

//------------------------------------------------------------------------------
// Name: update_sprite_registers
// Note: occurs at cycles 257 - 320 of all rendering scanlines
//------------------------------------------------------------------------------
void update_sprite_registers() {
	// this gets set to $00 for each tick between 257 and 320
	sprite_address_ = 0;
}

//------------------------------------------------------------------------------
// Name: update_vram_address
// Note: occurs at cycles 279 - 304 of prerender if screen is enabled
//------------------------------------------------------------------------------
void update_vram_address() {
	// v=t
	vram_address_ = (vram_address_ & ~0b0111101111100000) | (nametable_ & 0b0111101111100000);
}

//------------------------------------------------------------------------------
// Name: write4014
//------------------------------------------------------------------------------
bool rendering() {
	return vpos_ <= 240;
}

//------------------------------------------------------------------------------
// Name: increment_vram_address
//------------------------------------------------------------------------------
void increment_vram_address() {
	if (rendering() && ppu_mask_.screen_enabled) {
		if (ppu_control_.address_increment) {
			clock_y();
		} else {
			clock_x();
		}
	} else {
		if (ppu_control_.address_increment) {
			vram_address_ += 32;
		} else {
			vram_address_ += 1;
		}
	}
}

//------------------------------------------------------------------------------
// Name: clock_ppu
//------------------------------------------------------------------------------
void clock_ppu(const scanline_prerender &) {

	if (UNLIKELY(hpos_ == 0)) {
		status_.sprite0  = 0;
		status_.overflow = 0;
	} else if (UNLIKELY(hpos_ == 1)) {
		// clear all the relevant status bits
		status_.vblank = 0;
		write_block_   = false;
	}

	if (LIKELY(ppu_mask_.screen_enabled)) {
		if (UNLIKELY(hpos_ < 1)) {
			// idle
		} else if (LIKELY(hpos_ < 257)) {
			switch (hpos_ & 0x07) {
			case 1:
				evaluate_sprites_odd();
				open_tile_index();
				break;
			case 2:
				evaluate_sprites_even();
				read_tile_index();
				break;
			case 3:
				evaluate_sprites_odd();
				open_background_attribute();
				break;
			case 4:
				evaluate_sprites_even();
				read_background_attribute();
				break;
			case 5:
				evaluate_sprites_odd();
				open_background_pattern<pattern_0>();
				break;
			case 6:
				evaluate_sprites_even();
				read_background_pattern<pattern_0>();
				break;
			case 7:
				evaluate_sprites_odd();
				open_background_pattern<pattern_1>();
				break;
			case 0:
				evaluate_sprites_even();
				read_background_pattern<pattern_1>();
				update_shift_registers_idle();
				clock_x();
				break;
			}

			if (UNLIKELY(hpos_ == 256)) {
				clock_y();
			}
		} else if (hpos_ < 281) {

			if (UNLIKELY(hpos_ == 257)) {
				update_x_scroll();
			}

			update_sprite_registers();

			switch (hpos_ & 0x07) {
			case 1:
				open_tile_index();
				break; // open the bus for nametable fetch (garbage)
			case 2:
				read_tile_index();
				break; // fetch the name table byte (garbage)
			case 3:
				open_background_attribute();
				break; // open the bus for the attribute fetch (garbage)
			case 4:
				read_background_attribute();
				break; // fetch the attributes (garbage)
			case 5:
				if (ppu_control_.large_sprites) {
					open_sprite_pattern<size_16px, pattern_0>();
				} else {
					open_sprite_pattern<size_8px, pattern_0>();
				}
				break;
			case 6:
				if (ppu_control_.large_sprites) {
					read_sprite_pattern<size_16px, pattern_0>();
				} else {
					read_sprite_pattern<size_8px, pattern_0>();
				}
				break;
			case 7:
				if (ppu_control_.large_sprites) {
					open_sprite_pattern<size_16px, pattern_1>();
				} else {
					open_sprite_pattern<size_8px, pattern_1>();
				}
				break;
			case 8:
				if (ppu_control_.large_sprites) {
					read_sprite_pattern<size_16px, pattern_1>();
				} else {
					read_sprite_pattern<size_8px, pattern_1>();
				}
				break;
			}
		} else if (hpos_ < 305) {

			update_vram_address();
			update_sprite_registers();

			switch (hpos_ & 0x07) {
			case 1:
				open_tile_index();
				break; // open the bus for nametable fetch (garbage)
			case 2:
				read_tile_index();
				break; // fetch the name table byte (garbage)
			case 3:
				open_background_attribute();
				break; // open the bus for the attribute fetch (garbage)
			case 4:
				read_background_attribute();
				break; // fetch the attributes (garbage)
			case 5:
				if (ppu_control_.large_sprites) {
					open_sprite_pattern<size_16px, pattern_0>();
				} else {
					open_sprite_pattern<size_8px, pattern_0>();
				}
				break;
			case 6:
				if (ppu_control_.large_sprites) {
					read_sprite_pattern<size_16px, pattern_0>();
				} else {
					read_sprite_pattern<size_8px, pattern_0>();
				}
				break;
			case 7:
				if (ppu_control_.large_sprites) {
					open_sprite_pattern<size_16px, pattern_1>();
				} else {
					open_sprite_pattern<size_8px, pattern_1>();
				}
				break;
			case 0:
				if (ppu_control_.large_sprites) {
					read_sprite_pattern<size_16px, pattern_1>();
				} else {
					read_sprite_pattern<size_8px, pattern_1>();
				}
				break;
			}
		} else if (hpos_ < 321) {

			update_sprite_registers();

			switch (hpos_ & 0x07) {
			case 1:
				open_tile_index();
				break; // open the bus for nametable fetch (garbage)
			case 2:
				read_tile_index();
				break; // fetch the name table byte (garbage)
			case 3:
				open_background_attribute();
				break; // open the bus for the attribute fetch (garbage)
			case 4:
				read_background_attribute();
				break; // fetch the attributes (garbage)
			case 5:
				if (ppu_control_.large_sprites) {
					open_sprite_pattern<size_16px, pattern_0>();
				} else {
					open_sprite_pattern<size_8px, pattern_0>();
				}
				break;
			case 6:
				if (ppu_control_.large_sprites) {
					read_sprite_pattern<size_16px, pattern_0>();
				} else {
					read_sprite_pattern<size_8px, pattern_0>();
				}
				break;
			case 7:
				if (ppu_control_.large_sprites) {
					open_sprite_pattern<size_16px, pattern_1>();
				} else {
					open_sprite_pattern<size_8px, pattern_1>();
				}
				break;
			case 0:
				if (ppu_control_.large_sprites) {
					read_sprite_pattern<size_16px, pattern_1>();
				} else {
					read_sprite_pattern<size_8px, pattern_1>();
				}
				break;
			}
		} else if (hpos_ < 337) {
			// fetch first 2 tiles of NEXT scanline
			switch (hpos_ & 0x07) {
			case 1:
				open_tile_index();
				break; // open the bus for nametable fetch
			case 2:
				read_tile_index();
				break; // fetch the name table byte
			case 3:
				open_background_attribute();
				break; // open the bus for the attribute fetch
			case 4:
				read_background_attribute();
				break; // fetch the attributes
			case 5:
				open_background_pattern<pattern_0>();
				break; // open the bus for pattern A fetch
			case 6:
				read_background_pattern<pattern_0>();
				break; // read 1st pattern byte from 000PTTTTTTTT0YYY
			case 7:
				open_background_pattern<pattern_1>();
				break; // open the bus for pattern B fetch
			case 0:
				read_background_pattern<pattern_1>();
				update_shift_registers_idle();
				clock_x();
				break; // read 2nd pattern byte from 000PTTTTTTTT1YYY
			}
		} else {
			switch (hpos_) {
			// dummy fetches
			case 337:
				open_tile_index();
				break;
			case 338:
				read_tile_index();
				break;
			case 339:
				open_tile_index();
				if (odd_frame_) {
					++hpos_;
				}
				break; // skip one clock if the first visible line on odd frames
			case 340:
				read_tile_index();
				break;
			default:
				abort();
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name: clock_ppu
//------------------------------------------------------------------------------
void clock_ppu(const scanline_render &target) {

	if (UNLIKELY(!ppu_mask_.screen_enabled)) {

		if (UNLIKELY(hpos_ < 1)) {
			// idle
		} else if (LIKELY(hpos_ < 257)) {

			target.buffer[hpos_ - 1] = render_blank_pixel();
			target.buffer[hpos_ - 1] |= (ppu_mask_.intensity << 6);
		} else {
			// idle
		}
	} else {

		if (UNLIKELY(hpos_ < 1)) {
			// the first clock is acts like the last clock of the pre-render if we skipped a cycle
			if (UNLIKELY(vpos_ == 1 && odd_frame_)) {
				read_tile_index();
			} else {
				// idle
			}
		} else if (LIKELY(hpos_ < 257)) {

			// NOTE(eteran): on my machine, this code "costs" about 200 FPS
			target.buffer[hpos_ - 1] = render_pixel();
			target.buffer[hpos_ - 1] |= (ppu_mask_.intensity << 6);

			switch (hpos_ & 0x07) {
			case 1:
				evaluate_sprites_odd();
				open_tile_index();
				break;
			case 2:
				evaluate_sprites_even();
				read_tile_index();
				break;
			case 3:
				evaluate_sprites_odd();
				open_background_attribute();
				break;
			case 4:
				evaluate_sprites_even();
				read_background_attribute();
				break;
			case 5:
				evaluate_sprites_odd();
				open_background_pattern<pattern_0>();
				break;
			case 6:
				evaluate_sprites_even();
				read_background_pattern<pattern_0>();
				break;
			case 7:
				evaluate_sprites_odd();
				open_background_pattern<pattern_1>();
				break;
			case 0:
				evaluate_sprites_even();
				read_background_pattern<pattern_1>();
				update_shift_registers_render();
				clock_x();
				break;
			}

			if (UNLIKELY(hpos_ == 256)) {
				clock_y();
			}
		} else if (hpos_ < 321) {

			if (UNLIKELY(hpos_ == 257)) {
				update_x_scroll();
			}

			update_sprite_registers();

			// NOTE(eteran): on my machine, this code "costs" about 100 FPS
			switch (hpos_ & 0x07) {
			case 1:
				open_tile_index();
				break; // open the bus for nametable fetch (garbage)
			case 2:
				read_tile_index();
				break; // fetch the name table byte (garbage)
			case 3:
				open_background_attribute();
				break; // open the bus for the attribute fetch (garbage)
			case 4:
				read_background_attribute();
				break; // fetch the attributes (garbage)
			case 5:
				if (ppu_control_.large_sprites) {
					open_sprite_pattern<size_16px, pattern_0>();
				} else {
					open_sprite_pattern<size_8px, pattern_0>();
				}
				break;
			case 6:
				if (ppu_control_.large_sprites) {
					read_sprite_pattern<size_16px, pattern_0>();
				} else {
					read_sprite_pattern<size_8px, pattern_0>();
				}
				break;
			case 7:
				if (ppu_control_.large_sprites) {
					open_sprite_pattern<size_16px, pattern_1>();
				} else {
					open_sprite_pattern<size_8px, pattern_1>();
				}
				break;
			case 0:
				if (ppu_control_.large_sprites) {
					read_sprite_pattern<size_16px, pattern_1>();
				} else {
					read_sprite_pattern<size_8px, pattern_1>();
				}
				break;
			}
		} else if (hpos_ < 337) {
			// NOTE(eteran): on my machine, this code "costs" about 50 FPS
			// fetch first 2 tiles of NEXT scanline
			switch (hpos_ & 0x07) {
			case 1:
				open_tile_index();
				break; // open the bus for nametable fetch
			case 2:
				read_tile_index();
				break; // fetch the name table byte
			case 3:
				open_background_attribute();
				break; // open the bus for the attribute fetch
			case 4:
				read_background_attribute();
				break; // fetch the attributes
			case 5:
				open_background_pattern<pattern_0>();
				break; // open the bus for pattern A fetch
			case 6:
				read_background_pattern<pattern_0>();
				break; // read 1st pattern byte from 000PTTTTTTTT0YYY
			case 7:
				open_background_pattern<pattern_1>();
				break; // open the bus for pattern B fetch
			case 0:
				read_background_pattern<pattern_1>();
				update_shift_registers_idle();
				clock_x();
				break; // read 2nd pattern byte from 000PTTTTTTTT1YYY
			}
		} else {
			switch (hpos_) {
			// dummy fetches
			case 337:
				open_tile_index();
				break;
			case 338:
				read_tile_index();
				break;
			case 339:
				open_tile_index();
				break;
			case 340:
				read_tile_index();
				break;
			default:
				abort();
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name: clock_ppu
//------------------------------------------------------------------------------
void clock_ppu(const scanline_postrender &target) {
	(void)target;
}

//------------------------------------------------------------------------------
// Name: clock_ppu
//------------------------------------------------------------------------------
void clock_ppu(const scanline_vblank &target) {

	(void)target;

	// I know this should be 241 in theory, but we consider the pre-rendering
	// scanline to be #0 for now
	if (UNLIKELY(vpos_ == 242)) {
		switch (hpos_) {
		case 1:
			enter_vblank();
			break;
		case 3:
			if (ppu_control_.nmi_on_vblank && status_.vblank) {
				cpu::nmi();
			}
			break;
		}
	}
}

//------------------------------------------------------------------------------
// Name: start_frame
//------------------------------------------------------------------------------
void start_frame() {
	vpos_ = 0;
}

//------------------------------------------------------------------------------
// Name: end_frame
//------------------------------------------------------------------------------
void end_frame() {

	odd_frame_ = !odd_frame_;

	// we use the upper bits to count frames, so the upper byte should be about 0x3c
	// within one second
	latch_ += 0x100;
	if (latch_ > 0x3c00) {
		latch_ = 0;
	}

	cart.mapper()->ppu_end_frame();
}

}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void reset(Reset reset_type) {

	if (reset_type == Reset::Hard) {
		std::fill_n(sprite_ram_, 0x0100, 0);
		std::fill_n(sprite_data_, 32, 0xff);
		std::copy(std::begin(powerup_palette), std::end(powerup_palette), palette_);

		for (int i = 0; i < 8; ++i) {
			sprite_patterns_[i].patterns[0] = 0;
			sprite_patterns_[i].patterns[1] = 0;
			sprite_patterns_[i].x           = 0;
			sprite_patterns_[i].y           = 0;
			sprite_patterns_[i].index       = 0;
			sprite_patterns_[i].attr        = 0;
		}
	}

	attribute_queue_[0]   = 0;
	attribute_queue_[1]   = 0;
	hpos_                 = 0;
	latch_                = 0;
	nametable_            = 0x0000;
	next_attribute_       = 0;
	next_pattern_[0]      = 0;
	next_pattern_[1]      = 0;
	next_tile_index_      = 0;
	odd_frame_            = false;
	pattern_queue_[0]     = 0;
	pattern_queue_[1]     = 0;
	ppu_cycle_            = 0;
	ppu_control_.raw      = 0;
	ppu_mask_.raw         = 0;
	register_2007_buffer_ = 0;
	sprite_address_       = 0;
	sprite_data_index_    = 0;
	left_most_sprite_x_   = 0xff;
	status_.raw           = 0;
	tile_offset_          = 0;
	vpos_                 = 0;
	vram_address_         = 0x0000;
	write_latch_          = false;
	write_block_          = true;
	monochrome_mask_      = 0xff;

	std::cout << "PPU reset complete" << std::endl;
}

//------------------------------------------------------------------------------
// Name: write2000
//------------------------------------------------------------------------------
void write2000(uint8_t value) {

	latch_ = value;

	if (write_block_) {
		return;
	}

	const Control prev_control = ppu_control_;
	ppu_control_.raw           = value;

	// name table address
	// t:0000110000000000=d:00000011
	nametable_ &= 0b1111001111111111;
	nametable_ |= ((value & 0b00000011) << 10);

	if (prev_control.nmi_on_vblank && !ppu_control_.nmi_on_vblank) {
		cpu::clear_nmi();
	} else if (!prev_control.nmi_on_vblank && ppu_control_.nmi_on_vblank && status_.vblank && hpos_ != 0) {
		cpu::nmi();
	}
}

//------------------------------------------------------------------------------
// Name: write2001
//------------------------------------------------------------------------------
void write2001(uint8_t value) {
	latch_ = value;

	if (write_block_) {
		return;
	}

	ppu_mask_.raw = value;

	monochrome_mask_ = (ppu_mask_.monochrome) ? 0x30 : 0xff;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void write2002(uint8_t value) {
	latch_ = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void write2003(uint8_t value) {
	latch_          = value;
	sprite_address_ = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void write2004(uint8_t value) {
	latch_ = value;

	// sprite_address_ is an 8-bit type, so wrapping is implicit
	sprite_ram_[sprite_address_++] = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void write2005(uint8_t value) {
	latch_ = value;

	if (write_block_) {
		return;
	}

	write_latch_ = !write_latch_;

	if (write_latch_) {
		// 2005 first write:
		// t:0000000000011111=d:11111000
		// x=d:00000111
		nametable_ &= 0b111111111100000;
		nametable_ |= (value & 0b11111000) >> 3;
		tile_offset_ = value & 0x07;
	} else {
		// 2005 second write:
		// t:0000001111100000=d:11111000
		// t:0111000000000000=d:00000111
		nametable_ &= ~0b0111001111100000;
		nametable_ |= (value & 0b11111000) << 2;
		nametable_ |= (value & 0b00000111) << 12;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void write2006(uint8_t value) {
	latch_ = value;

	if (write_block_) {
		return;
	}

	write_latch_ = !write_latch_;

	if (write_latch_) {
		// 2006 first write:
		// t:0011111100000000=d:00111111
		// t:1100000000000000=0
		nametable_ &= 0b0000000011111111;
		nametable_ |= (value & 0b00111111) << 8;
	} else {
		// 2006 second write:
		// t:0000000011111111=d:11111111
		// v=t
		nametable_ &= 0b0111111100000000;
		nametable_ |= (value & 0b11111111);
		vram_address_ = nametable_;

		cart.mapper()->vram_change_hook(vram_address_);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void write2007(uint8_t value) {
	latch_ = value;

	const uint16_t temp_address = vram_address_ & 0b0011111111111111;

	increment_vram_address();

	cart.mapper()->vram_change_hook(vram_address_);

	// palette write
	if ((temp_address & 0b0011111100000000) == 0b0011111100000000) {

		const uint16_t palette_address = temp_address & 0x1f;
		palette_[palette_address]      = value & 0x3f;

		if ((palette_address & 0x03) == 0x00) {
			// $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C.
			palette_[palette_address ^ 0x10] = value & 0x3f;
		}

	} else {
		cart.mapper()->write_vram(temp_address, value);
	}
}

//------------------------------------------------------------------------------
// Name: read200x
//------------------------------------------------------------------------------
uint8_t read200x() {
	return static_cast<uint8_t>(latch_);
}

//------------------------------------------------------------------------------
// Name: read2002
//------------------------------------------------------------------------------
uint8_t read2002() {

	// upper 3 bits of status
	// lower 5 bits of garbage (latch)
	const uint8_t ret =
		((status_.raw & (StatusOverflow | StatusSprite0 | StatusVBlank)) |
		 (latch_ & ~(StatusOverflow | StatusSprite0 | StatusVBlank))) &
		0xff;

	// reset scroll/write latch
	write_latch_ = false;

	// clear vblank flag
	status_.vblank = false;

	ppu_read_2002_cycle_ = ppu_cycle_;

	return ret;
}

//------------------------------------------------------------------------------
// Name: read2004
//------------------------------------------------------------------------------
uint8_t read2004() {

	if (!rendering() || !ppu_mask_.screen_enabled) {
		switch (sprite_address_ & 0x03) {
		case 0x00:
			latch_ = sprite_ram_[sprite_address_] & 0xff;
			break;
		case 0x01:
			latch_ = sprite_ram_[sprite_address_] & 0xff;
			break;
		case 0x02:
			latch_ = sprite_ram_[sprite_address_] & 0xe3;
			break;
		case 0x03:
			latch_ = sprite_ram_[sprite_address_] & 0xff;
			break;
		}

		return latch_ & 0xff;
	}

	return 0x00;
}

//------------------------------------------------------------------------------
// Name: read2007
//------------------------------------------------------------------------------
uint8_t read2007() {

	if (write_block_) {
		return 0x00;
	}

	const uint16_t temp_address = vram_address_ & 0b0011111111111111;

	increment_vram_address();

	cart.mapper()->vram_change_hook(vram_address_);

	const auto decay_value = static_cast<uint8_t>(latch_);

	latch_                = register_2007_buffer_;
	register_2007_buffer_ = cart.mapper()->read_vram(temp_address);

	if ((temp_address & 0b0011111100000000) == 0b0011111100000000) {

		latch_ = palette_[temp_address & 0x1f] | (decay_value & 0xc0);
		if (UNLIKELY(ppu_mask_.monochrome)) {
			latch_ &= 0xf0;
		}
	}

	return latch_ & 0xff;
}

//------------------------------------------------------------------------------
// Name: write4014
//------------------------------------------------------------------------------
void write4014(uint8_t value) {
	// drain current cycles, then go ahead and do the DMA
	// the procedure takes 513 CPU cycles (+1 on odd CPU cycles):
	// first one (or two) idle cycles, and then 256 pairs of alternating
	// read/write cycles.
	const auto sprite_addr = static_cast<uint16_t>(value << 8);
	cpu::schedule_spr_dma(write2004, sprite_addr, 256);
}

//------------------------------------------------------------------------------
// Name: execute_scanline
//------------------------------------------------------------------------------
template <class T>
void execute_scanline(const T &target) {

	if (UNLIKELY(vpos_ == 262)) {
		start_frame();
	} else if (UNLIKELY(vpos_ == 241)) {
		end_frame();
	}

	if (LIKELY(!system_paused)) {
		for (hpos_ = 0; hpos_ < CyclesPerScanline; ++hpos_, ++ppu_cycle_) {
			clock_ppu(target);
			if ((ppu_cycle_ % 3) == CpuAlignment) {
				cpu::exec<1>();
				apu::exec<1>();
			}
		}
		++vpos_;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint64_t cycle_count() {
	return ppu_cycle_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint16_t hpos() {
	return hpos_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint16_t vpos() {
	return vpos_;
}

// explicitly instantiate the types we use for this function,
// we don't want to have to put this code in the header
template void execute_scanline<scanline_vblank>(const scanline_vblank &target);
template void execute_scanline<scanline_prerender>(const scanline_prerender &target);
template void execute_scanline<scanline_postrender>(const scanline_postrender &target);
template void execute_scanline<scanline_render>(const scanline_render &target);

}
}
