
#ifndef PPU_20080314_H_
#define PPU_20080314_H_

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

namespace nes::ppu {

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
uint_least16_t hpos();
uint_least16_t vpos();

extern bool show_sprites;
extern bool system_paused;

}

#endif
