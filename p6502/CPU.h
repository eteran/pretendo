
#ifndef CPU_20080314_H_
#define CPU_20080314_H_

#include "../Reset.h"
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

void write(uint16_t address, uint8_t value);
uint8_t read(uint16_t address);

uint16_t pc();
uint8_t a();
uint8_t p();
uint8_t s();
uint8_t x();
uint8_t y();

// uneccessary, but can help keep debugger code clean
bool flag_c();
bool flag_d();
bool flag_i();
bool flag_n();
bool flag_v();
bool flag_z();

uint64_t cycle_count();
void clear_irq(IRQ_SOURCE source);
void exec(int cycles);
void irq(IRQ_SOURCE source);
void nmi();
void schedule_spr_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count);
void schedule_dmc_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count);

void swap_01(uint8_t *ptr);
void swap_23(uint8_t *ptr);
void swap_45(uint8_t *ptr);
void swap_67(uint8_t *ptr);
void swap_89(uint8_t *ptr);
void swap_ab(uint8_t *ptr);
void swap_cd(uint8_t *ptr);
void swap_ef(uint8_t *ptr);

void unmap_01();
void unmap_23();
void unmap_45();
void unmap_67();
void unmap_89();
void unmap_ab();
void unmap_cd();
void unmap_ef();

void trash_ram();
void reset(Reset reset_type);

}
}

#endif
