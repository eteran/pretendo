
#ifndef PPU_20080314_H_
#define PPU_20080314_H_

#include "Reset.h"
#include "BitField.h"
#include <cstdint>

class Mapper;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

struct scanline {
};

struct scanline_vblank : scanline {
};

struct scanline_prerender : scanline {
};

struct scanline_postrender : scanline {
};

struct scanline_render : scanline {
	explicit scanline_render(uint8_t *p) : buffer(p) {
	}
	uint8_t *const buffer;
};

namespace nes {
namespace ppu {

union PPUControl {
	uint8_t raw;
	BitField<0,2> nametable;
	BitField<2>   address_increment;
	BitField<3>   sprite_pattern_table;
	BitField<4>   background_pattern_table;
	BitField<5>   large_sprites;
	BitField<6>   master;
	BitField<7>   nmi_on_vblank;
};

union PPUMask {
	uint8_t raw;
	BitField<0>   monochrome;
	BitField<1>   background_clipping;
	BitField<2>   sprite_clipping;
	BitField<3>   background_visible;
	BitField<4>   sprites_visible;
	BitField<5,3> intensity;

	// meta-fields which don't occupy any space :-)
	BitField<3,2> screen_enabled;
};

void reset(Reset reset_type);

constexpr uint8_t mirror_single_low  = 0x00;
constexpr uint8_t mirror_single_high = 0x55;
constexpr uint8_t mirror_vertical    = 0x44;
constexpr uint8_t mirror_horizontal  = 0x50;
constexpr uint8_t mirror_4screen     = 0xe4;

void write2000(uint8_t value);
void write2001(uint8_t value);
void write2002(uint8_t value);
void write2003(uint8_t value);
void write2004(uint8_t value);
void write2005(uint8_t value);
void write2006(uint8_t value);
void write2007(uint8_t value);
void write4014(uint8_t value);

uint8_t read2002();
uint8_t read2004();
uint8_t read2007();
uint8_t read200x();

void write_vram(uint16_t address, uint8_t value);
uint8_t read_vram(uint16_t address);

void end_frame();

const PPUMask &mask();
const PPUControl &control();

template <class T>
void execute_scanline(const T &target);

void set_mirroring(uint8_t mir);
void set_vram_bank(uint8_t bank, uint8_t *p, bool writeable);
void start_frame();
void unset_vram_bank(uint8_t bank);

uint64_t cycle_count();
uint16_t hpos();
uint16_t vpos();

extern bool show_sprites_;

}
}

#endif
