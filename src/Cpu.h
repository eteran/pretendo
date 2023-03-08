
#ifndef CPU_20080314_H_
#define CPU_20080314_H_

#include "BitField.h"
#include "Reset.h"
#include <cstdint>

namespace nes {
namespace cpu {

using dma_handler_t = void (*)(uint8_t);

enum IrqSource : uint8_t {
	MAPPER_IRQ = 0x01,
	APU_IRQ    = 0x02,
	FDS_IRQ    = 0x04,
	ALL_IRQ    = 0xff
};

enum : uint8_t {
	C_MASK = 0x01,
	Z_MASK = 0x02,
	I_MASK = 0x04,
	D_MASK = 0x08,
	B_MASK = 0x10,
	R_MASK = 0x20, // antisocial flag... always 1
	V_MASK = 0x40,
	N_MASK = 0x80
};

union register16 {
	uint16_t raw;
	BitField<uint16_t, 0, 8> lo;
	BitField<uint16_t, 8, 8> hi;
};

// API
uint64_t cycle_count();
void clear_irq(IrqSource source);
void clear_nmi();
void irq(IrqSource source);
void schedule_spr_dma(dma_handler_t dma_handler, uint32_t source_address, uint32_t count);
void schedule_dmc_dma(dma_handler_t dma_handler, uint32_t source_address, uint32_t count);
void reset(Reset reset_type);
void reset();
void stop();
void nmi();
void tick();

// public registers
extern register16 PC;
extern uint8_t A;
extern uint8_t X;
extern uint8_t Y;
extern uint8_t S;
extern uint8_t P;

template <int Cycles>
void exec() {
	for (int i = 0; i < Cycles; ++i) {
		tick();
	}
}

}
}

#endif
