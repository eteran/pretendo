
#ifndef P6502_20080314_H_
#define P6502_20080314_H_

#include "bitfield.h"
#include <cstdint>

namespace P6502 {

using std::int8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

using dma_handler_t = void (*)(uint8_t);

union register16 {
	uint16_t raw;
	bitfield<0,8> lo;
	bitfield<8,8> hi;
};

// NOTE(eteran): user of this library must implement the following...
uint8_t read_handler(uint16_t address);
void write_handler(uint16_t address, uint8_t value);
void jam_handler();
void sync_handler();


enum DMA_SOURCE {
	SPR_DMA = 0,
	DMC_DMA = 1
};

// public registers
extern register16 PC;
extern uint8_t    A;
extern uint8_t    X;
extern uint8_t    Y;
extern uint8_t    S;
extern uint8_t    P;

// stats
extern uint64_t executed_cycles;

void init();
void reset();
void stop();
void irq();
void nmi();
void reset_irq();
void reset_nmi();
void run(int cycles);
void tick();
void schedule_dma(dma_handler_t dma_handler, uint16_t source_address, uint16_t count, DMA_SOURCE source);

template <int Cycles>
void run() {
	for(int i = 0; i < Cycles; ++i) {
		tick();
	}
}

}

#endif
