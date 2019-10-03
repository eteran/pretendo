
#ifndef PPU_20080314_H_
#define PPU_20080314_H_

#include "Reset.h"
#include "BitField.h"
#include "VRAMBank.h"
#include <cstdint>

class Mapper;

struct scanline_vblank     {};
struct scanline_prerender  {};
struct scanline_postrender {};

struct scanline_render {
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

void start_frame();
void end_frame();

const PPUMask &mask();
const PPUControl &control();

template <class T>
void execute_scanline(const T &target);

uint64_t cycle_count();
uint16_t hpos();
uint16_t vpos();

extern bool show_sprites_;
extern bool system_paused;


}
}

extern VRAMBank     vram_banks_[0x10];


#endif
