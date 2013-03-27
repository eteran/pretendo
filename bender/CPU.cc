
#include "CPU.h"
#include "../Mapper.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace {

//-------------------------------------------------------------------
// Name: read_handler
//-------------------------------------------------------------------
uint8_t read_handler(uint16_t address) {
	return nes::cart.mapper()->read_memory(address);
}

//-------------------------------------------------------------------
// Name: invalid_handler
//-------------------------------------------------------------------
void invalid_handler() {
	printf("INVALID OP\n");
	abort();
}

//-------------------------------------------------------------------
// Name: jam_handler
//-------------------------------------------------------------------
void jam_handler() {
	printf("JAM\n");
	abort();
}

//-------------------------------------------------------------------
// Name: write_handler
//-------------------------------------------------------------------
void write_handler(uint16_t address, uint8_t value) {
	return nes::cart.mapper()->write_memory(address, value);
}

}

//-------------------------------------------------------------------
// Name: CPU
//-------------------------------------------------------------------
CPU::CPU() : cycle_(0), irq_sources_(0x00) {

	bender_init((void*)read_handler, (void*)write_handler, (void*)jam_handler);

	//for(size_t i = 0; i < PAGE_COUNT; ++i) {
	//	bender_memory[i] = 0;
	//}

	swap_01(ram_);
}

//-------------------------------------------------------------------
// Name: ~CPU
//-------------------------------------------------------------------
CPU::~CPU() {
}


//-------------------------------------------------------------------
// Name: reset
//-------------------------------------------------------------------
void CPU::reset() {

	bender_reset();
	std::cout << "[CPU::reset] reset complete" << std::endl;
}

//-------------------------------------------------------------------
// Name: write
//-------------------------------------------------------------------
void CPU::write(uint16_t address, uint8_t value) {

	if(address >= 0x2000) {
		if(bender_memory[address >> page_shift] != 0) {
			bender_memory[address >> page_shift][address & page_mask] = value;
		}
	} else {
		ram_[address & 0x07ff] = value; // really the core should handle this internally
	}
}

//-------------------------------------------------------------------
// Name: read
//-------------------------------------------------------------------
uint8_t CPU::read(uint16_t address) {

	if(address >= 0x2000) {
		if(bender_memory[address >> page_shift] != 0) {
			return bender_memory[address >> page_shift][address & page_mask];
		} else {
			return (address >> 8) & 0xff;
		}
	} else {
		return ram_[address & 0x07ff]; // see above.  you don't see, do you? oh well.
	}
}

//-------------------------------------------------------------------
// Name: exec
//-------------------------------------------------------------------
int CPU::exec(int cycles) {

	cycle_ += cycles;
	const int elapsed = bender_run(cycle_);
	nes::apu.run(elapsed);
	cycle_ -= elapsed;
	return elapsed;
}

//-------------------------------------------------------------------
// Name: stop
//-------------------------------------------------------------------
void CPU::stop() {

}

//-------------------------------------------------------------------
// Name: trash_ram
//-------------------------------------------------------------------
void CPU::trash_ram() {
	using std::rand;
	//std::generate_n(ram_, sizeof(ram_), rand);
	std::memset(ram_, 0, sizeof(ram_));
}

//-------------------------------------------------------------------
// Name: irq
//-------------------------------------------------------------------
void CPU::irq(IRQ_SOURCE source) {

	irq_sources_ |= source;

	if(irq_sources_) {
		bender_irq();
	}
}

//-------------------------------------------------------------------
// Name: clear_irq
//-------------------------------------------------------------------
void CPU::clear_irq(IRQ_SOURCE source) {

	irq_sources_ &= ~source;

	if(!irq_sources_) {
		bender_clearirq();
	}
}
