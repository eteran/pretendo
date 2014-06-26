
#include "CPU.h"
#include "../Mapper.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>

//-------------------------------------------------------------------
// Name: CPU
//-------------------------------------------------------------------
CPU::CPU() : irq_sources_(0x00) {

	P6502::init(
		context_,
		[]() {
			// JAM handler
		},
		[](uint16_t address) {
			// read handler
			return nes::cart.mapper()->read_memory(address);
		},
		[](uint16_t address, uint8_t value) {
			// write handler
			return nes::cart.mapper()->write_memory(address, value);
		},
		[]() {
			// sync handler
			return nes::cart.mapper()->cpu_sync();
		});

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

//-------------------------------------------------------------------
// Name: ~CPU
//-------------------------------------------------------------------
CPU::~CPU() {
}

//-------------------------------------------------------------------
// Name: reset
//-------------------------------------------------------------------
void CPU::reset(nes::RESET reset_type) {

	if(reset_type == nes::HARD_RESET) {
		P6502::stop(context_);
		trash_ram();
	}

	P6502::reset(context_);
	std::cout << "[CPU::reset] reset complete" << std::endl;
}

//-------------------------------------------------------------------
// Name: write
//-------------------------------------------------------------------
void CPU::write(uint16_t address, uint8_t value) {
	
	if(page_[address >> page_shift]) {
		page_[address >> page_shift][address & page_mask] = value;
	}
}

//-------------------------------------------------------------------
// Name: read
//-------------------------------------------------------------------
uint8_t CPU::read(uint16_t address) const {

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
void CPU::exec(int cycles) {
	P6502::run(context_, cycles);
	nes::apu.run(cycles);
}

//-------------------------------------------------------------------
// Name: trash_ram
//-------------------------------------------------------------------
void CPU::trash_ram() {
	using std::rand;
	//std::generate_n(ram_, sizeof(ram_), rand);
	std::memset(ram_, 0xff, sizeof(ram_));
}

//-------------------------------------------------------------------
// Name: irq
//-------------------------------------------------------------------
void CPU::irq(IRQ_SOURCE source) {

	irq_sources_ |= source;

	if(irq_sources_) {
		P6502::irq(context_);
	}
}

//-------------------------------------------------------------------
// Name: clear_irq
//-------------------------------------------------------------------
void CPU::clear_irq(IRQ_SOURCE source) {

	irq_sources_ &= ~source;

	if(!irq_sources_) {
		P6502::reset_irq(context_);
	}
}

//-------------------------------------------------------------------
// Name: schedule_spr_dma
//-------------------------------------------------------------------
void CPU::schedule_spr_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count) {
	P6502::schedule_dma(context_, dma_handler, source_address, count, P6502::SPR_DMA);
}


//-------------------------------------------------------------------
// Name: schedule_dmc_dma
//-------------------------------------------------------------------
void CPU::schedule_dmc_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count) {
	P6502::schedule_dma(context_, dma_handler, source_address, count, P6502::DMC_DMA);
}
