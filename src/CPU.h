
#ifndef CPU_20080314_H_
#define CPU_20080314_H_

#include "Reset.h"
#include "P6502.h"
#include <memory>
#include <cstdint>

class Mapper;

namespace nes {
namespace cpu {

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

enum IRQ_SOURCE {
	MAPPER_IRQ = 0x01,
	APU_IRQ    = 0x02,
	FDS_IRQ    = 0x04,
	ALL_IRQ    = 0xff
};

uint64_t cycle_count();
void clear_irq(IRQ_SOURCE source);
void clear_nmi();
void irq(IRQ_SOURCE source);
void nmi();
void schedule_spr_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count);
void schedule_dmc_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count);

template <int N>
void exec() {
	P6502::run<N>();
}

void reset(Reset reset_type);

}
}

#endif
