
#ifndef SAVE_STATE_H_
#define SAVE_STATE_H_

#include <cstdint>

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

	uint8_t irq_sources;

	// internal registers
	uint16_t instruction;
	int cycle;

	// internal registers (which get trashed by instructions)
	uint16_t effective_address16;
	uint16_t data16;
	uint16_t old_pc;
	uint16_t new_pc;
	uint8_t data8;

	bool irq_asserted;
	bool nmi_asserted;
	bool rst_asserted;
	bool irq_executing;
	bool nmi_executing;
	bool rst_executing;

	void *spr_dma_handler;
	uint16_t spr_dma_source_address;
	uint16_t spr_dma_count;
	uint8_t spr_dma_byte;
	uint8_t spr_dma_delay;

	void *dmc_dma_handler;
	uint16_t dmc_dma_source_address;
	uint16_t dmc_dma_count;
	uint8_t dmc_dma_byte;
	uint8_t dmc_dma_delay;

	// stats
	uint64_t executed_cycles = 1; // NOTE(eteran): 1 instead of 0 makes 4.irq_and_dma.nes pass...
};

#endif
