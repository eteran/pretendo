
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

union register16 {
	uint16_t raw;
	bitfield<0,8> lo;
	bitfield<8,8> hi;
};


namespace {

typedef uint8_t (*read_handler_t)(uint16_t);
typedef void (*jam_handler_t)();
typedef void (*sync_handler_t)();
typedef void (*write_handler_t)(uint16_t, uint8_t);
typedef void (*dma_handler_t)(uint8_t);

}

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

void init(jam_handler_t jam, read_handler_t read, write_handler_t write, sync_handler_t sync);
void reset();
void stop();
void irq();
void nmi();
void reset_irq();
void run(int cycles);
void schedule_dma(dma_handler_t dma_handler, uint16_t source_address, uint16_t count, DMA_SOURCE source);

}

#endif
