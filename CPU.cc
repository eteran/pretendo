
#include "CPU.h"
#include "../APU.h"
#include "../NES.h"
#include "../Cart.h"
#include "../Mapper.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>


// implement the required functions
namespace P6502 {

uint8_t read_handler(uint16_t address) {
	return nes::cart.mapper()->read_memory(address);
}

void write_handler(uint16_t address, uint8_t value) {
	return nes::cart.mapper()->write_memory(address, value);
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

enum : uint8_t {
	C_MASK = 0x01,
	Z_MASK = 0x02,
	I_MASK = 0x04,
	D_MASK = 0x08,
	B_MASK = 0x10,
	R_MASK = 0x20,	// antisocial flag... always 1
	V_MASK = 0x40,
	N_MASK = 0x80
};

static constexpr unsigned int page_count = 32;
static constexpr unsigned int page_size  = 0x10000 / page_count;
static constexpr unsigned int page_mask  = page_size - 1;
static constexpr unsigned int page_shift = 11;

uint8_t ram_[0x800];
uint8_t irq_sources_ = 0x00;
uint8_t *page_[page_count];

namespace {

struct static_initializer {
	static_initializer() {
		P6502::init();

		page_[0x00] = ram_;
		page_[0x01] = ram_;
		page_[0x02] = ram_;
		page_[0x03] = ram_;
		page_[0x04] = 0;
		page_[0x05] = 0;
		page_[0x06] = 0;
		page_[0x07] = 0;
		page_[0x08] = 0;
		page_[0x09] = 0;
		page_[0x0a] = 0;
		page_[0x0b] = 0;
		page_[0x0c] = 0;
		page_[0x0d] = 0;
		page_[0x0e] = 0;
		page_[0x0f] = 0;
		page_[0x10] = 0;
		page_[0x11] = 0;
		page_[0x12] = 0;
		page_[0x13] = 0;
		page_[0x14] = 0;
		page_[0x15] = 0;
		page_[0x16] = 0;
		page_[0x17] = 0;
		page_[0x18] = 0;
		page_[0x19] = 0;
		page_[0x1a] = 0;
		page_[0x1b] = 0;
		page_[0x1c] = 0;
		page_[0x1d] = 0;
		page_[0x1e] = 0;
		page_[0x1f] = 0;	
	}
} static_initializer_;

}

//-------------------------------------------------------------------
// Name: reset
//-------------------------------------------------------------------
void reset(Reset reset_type) {

	if(reset_type == Reset::Hard) {
		P6502::stop();
		trash_ram();
	}

	P6502::reset();
	std::cout << "CPU reset complete" << std::endl;
}

//-------------------------------------------------------------------
// Name: write
//-------------------------------------------------------------------
void write(uint16_t address, uint8_t value) {

	if(page_[address >> page_shift]) {
		page_[address >> page_shift][address & page_mask] = value;
	}
}

//-------------------------------------------------------------------
// Name: read
//-------------------------------------------------------------------
uint8_t read(uint16_t address) {

	if(page_[address >> page_shift]) {
		return page_[address >> page_shift][address & page_mask];
	} else {
		// simulate open bus
		return (address >> 8) & 0xff;
	}
}

//-------------------------------------------------------------------
// Name: exec
//-------------------------------------------------------------------
void exec(int cycles) {
	P6502::run(cycles);
	apu::run(cycles);
}

//-------------------------------------------------------------------
// Name: trash_ram
//-------------------------------------------------------------------
void trash_ram() {
	using std::rand;
	//std::generate_n(ram_, sizeof(ram_), rand);
	std::memset(ram_, 0xff, sizeof(ram_));
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

uint16_t pc() { return PC.raw;}
uint8_t a()   { return A; }
uint8_t p()   { return P; }
uint8_t s()   { return S; }
uint8_t x()   { return X; }
uint8_t y()   { return Y; }
uint64_t cycle_count() { return P6502::executed_cycles; }

bool flag_c() { return (p() & C_MASK) != 0; }
bool flag_d() { return (p() & D_MASK) != 0; }
bool flag_i() { return (p() & I_MASK) != 0; }
bool flag_n() { return (p() & N_MASK) != 0; }
bool flag_v() { return (p() & V_MASK) != 0; }
bool flag_z() { return (p() & Z_MASK) != 0; }
void nmi() { P6502::nmi(); }

void swap_01(uint8_t *ptr) { page_[0x00] = ptr + 0x0000; page_[0x01] = ptr + 0x0800; page_[0x02] = ptr + 0x1000; page_[0x03] = ptr + 0x1800; }
void swap_23(uint8_t *ptr) { page_[0x04] = ptr + 0x0000; page_[0x05] = ptr + 0x0800; page_[0x06] = ptr + 0x1000; page_[0x07] = ptr + 0x1800; }
void swap_45(uint8_t *ptr) { page_[0x08] = ptr + 0x0000; page_[0x09] = ptr + 0x0800; page_[0x0a] = ptr + 0x1000; page_[0x0b] = ptr + 0x1800; }
void swap_67(uint8_t *ptr) { page_[0x0c] = ptr + 0x0000; page_[0x0d] = ptr + 0x0800; page_[0x0e] = ptr + 0x1000; page_[0x0f] = ptr + 0x1800; }
void swap_89(uint8_t *ptr) { page_[0x10] = ptr + 0x0000; page_[0x11] = ptr + 0x0800; page_[0x12] = ptr + 0x1000; page_[0x13] = ptr + 0x1800; }
void swap_ab(uint8_t *ptr) { page_[0x14] = ptr + 0x0000; page_[0x15] = ptr + 0x0800; page_[0x16] = ptr + 0x1000; page_[0x17] = ptr + 0x1800; }
void swap_cd(uint8_t *ptr) { page_[0x18] = ptr + 0x0000; page_[0x19] = ptr + 0x0800; page_[0x1a] = ptr + 0x1000; page_[0x1b] = ptr + 0x1800; }
void swap_ef(uint8_t *ptr) { page_[0x1c] = ptr + 0x0000; page_[0x1d] = ptr + 0x0800; page_[0x1e] = ptr + 0x1000; page_[0x1f] = ptr + 0x1800; }

void unmap_01() { page_[0x00] = nullptr; page_[0x01] = nullptr; page_[0x02] = nullptr; page_[0x03] = nullptr; }
void unmap_23() { page_[0x04] = nullptr; page_[0x05] = nullptr; page_[0x06] = nullptr; page_[0x07] = nullptr; }
void unmap_45() { page_[0x08] = nullptr; page_[0x09] = nullptr; page_[0x0a] = nullptr; page_[0x0b] = nullptr; }
void unmap_67() { page_[0x0c] = nullptr; page_[0x0d] = nullptr; page_[0x0e] = nullptr; page_[0x0f] = nullptr; }
void unmap_89() { page_[0x10] = nullptr; page_[0x11] = nullptr; page_[0x12] = nullptr; page_[0x13] = nullptr; }
void unmap_ab() { page_[0x14] = nullptr; page_[0x15] = nullptr; page_[0x16] = nullptr; page_[0x17] = nullptr; }
void unmap_cd() { page_[0x18] = nullptr; page_[0x19] = nullptr; page_[0x1a] = nullptr; page_[0x1b] = nullptr; }
void unmap_ef() { page_[0x1c] = nullptr; page_[0x1d] = nullptr; page_[0x1e] = nullptr; page_[0x1f] = nullptr; }

}
}
