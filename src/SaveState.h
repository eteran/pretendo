
#ifndef SAVE_STATE_H_
#define SAVE_STATE_H_

#include <cstdint>

// TODO(eteran): implement internal state

struct SaveState {

	// System
	uint8_t sys_ram[0x800];

	// CPU
	uint16_t cpu_pc;
	uint8_t cpu_a;
	uint8_t cpu_x;
	uint8_t cpu_y;
	uint8_t cpu_s;
	uint8_t cpu_p;

	// PPU
	uint8_t ppu_sprite_ram[0x100];
	uint8_t ppu_palette[0x20];

	// API

	// Mapper


};

#endif
