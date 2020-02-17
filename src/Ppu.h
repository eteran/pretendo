
#ifndef PPU_20080314_H_
#define PPU_20080314_H_

#include "BitField.h"
#include "Reset.h"
#include <cstdint>

class Mapper;

struct scanline_vblank {};
struct scanline_prerender {};
struct scanline_postrender {};

struct scanline_render {
	explicit scanline_render(uint16_t *p)
		: buffer(p) {
	}
	uint16_t *const buffer;
};

namespace nes {
namespace ppu {

union Status {
	uint8_t raw;

	BitField<uint8_t, 5> overflow;
	BitField<uint8_t, 6> sprite0;
	BitField<uint8_t, 7> vblank;

	BitField<uint8_t, 5, 3> flags;
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

void reset(Reset reset_type);

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

template <class T>
void execute_scanline(const T &target);

uint64_t cycle_count();
uint16_t hpos();
uint16_t vpos();

extern bool show_sprites;
extern bool system_paused;

}
}

#endif
