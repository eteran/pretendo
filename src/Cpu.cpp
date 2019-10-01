
#include "Cpu.h"
#include "Nes.h"
#include "Bus.h"
#include "Cart.h"
#include "Mapper.h"
#include "Compiler.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>

// implement the required functions
namespace P6502 {

uint8_t read_handler(uint16_t address) {
	return nes::bus::read_memory(address);
}

void write_handler(uint16_t address, uint8_t value) {
	nes::bus::write_memory(address, value);
}

void jam_handler() {
}

void sync_handler() {
	return nes::cart.mapper()->cpu_sync();
}

}

namespace nes {
namespace cpu {

using P6502::PC;
using P6502::A; 
using P6502::P; 
using P6502::S; 
using P6502::X; 
using P6502::Y; 

namespace {

uint8_t irq_sources_ = 0x00;

namespace {

struct static_initializer {
	static_initializer() {
		P6502::init();
	}
} static_initializer_;

}

}

//-------------------------------------------------------------------
// Name: reset
//-------------------------------------------------------------------
void reset(Reset reset_type) {

	if(reset_type == Reset::Hard) {
		P6502::stop();
		nes::bus::trash_ram();
	}

	P6502::reset();
	std::cout << "CPU reset complete" << std::endl;
}



//-------------------------------------------------------------------
// Name: irq
//-------------------------------------------------------------------
void irq(IRQ_SOURCE source) {

	irq_sources_ |= source;

	if(irq_sources_) {
		P6502::irq();
	}
}

//-------------------------------------------------------------------
// Name: clear_irq
//-------------------------------------------------------------------
void clear_irq(IRQ_SOURCE source) {

	irq_sources_ &= ~source;

	if(!irq_sources_) {
		P6502::reset_irq();
	}
}

//-------------------------------------------------------------------
// Name: clear_nmi
//-------------------------------------------------------------------
void clear_nmi() {
	P6502::reset_nmi();
}

//-------------------------------------------------------------------
// Name: schedule_spr_dma
//-------------------------------------------------------------------
void schedule_spr_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count) {
	P6502::schedule_dma(dma_handler, source_address, count, P6502::SPR_DMA);
}


//-------------------------------------------------------------------
// Name: schedule_dmc_dma
//-------------------------------------------------------------------
void schedule_dmc_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count) {
	P6502::schedule_dma(dma_handler, source_address, count, P6502::DMC_DMA);
}

uint64_t cycle_count() { return P6502::executed_cycles; }
void nmi() { P6502::nmi(); }



}
}
