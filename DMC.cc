
#include "DMC.h"
#include "NES.h"
#include "Mapper.h"

namespace {

const uint16_t frequency_table[16] = {
	0x1ac, 0x17c, 0x154, 0x140,
	0x11e, 0x0fe, 0x0e2, 0x0d6,
	0x0be, 0x0a0, 0x08e, 0x080,
	0x06a, 0x054, 0x048, 0x036
};

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
DMC::DMC() : enabled_(false), frequency_(0xffff), reload_frequency_(0xffff), current_address_(0xc000), sample_address_(0xc000), bytes_remaining_(0), sample_length_(0), irq_enabled_(0x80), load_counter_(0), loop_(0), bit_(0) {
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
DMC::~DMC() {
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void DMC::enable() {
	enabled_ = true;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void DMC::disable() {
	enabled_       = false;
	sample_length_ = 0;
}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void DMC::reset() {
	/*
	enabled_        = false;
	frequency_      = 0xffff;
	reload_frequency_ = 0xffff;
	current_address_ = 0;
	sample_address_ = 0;
	bytes_remaining_  = 0;
	sample_length_  = 0;
	irq_enabled_    = 0x80;
	load_counter_   = 0;
	loop_           = 0;
	bit_            = 0;
	*/
}

//------------------------------------------------------------------------------
// Name: write_reg0
//------------------------------------------------------------------------------
void DMC::write_reg0(uint8_t value) {
	irq_enabled_      = value & 0x80;
	loop_             = value & 0x40;
	frequency_        = frequency_table[value & 0x0f];
	reload_frequency_ = frequency_table[value & 0x0f];
}

//------------------------------------------------------------------------------
// Name: write_reg1
//------------------------------------------------------------------------------
void DMC::write_reg1(uint8_t value) {
	load_counter_ = value & 0x7f;
}

//------------------------------------------------------------------------------
// Name: write_reg2
//------------------------------------------------------------------------------
void DMC::write_reg2(uint8_t value) {
	current_address_ = 0xc000 | (value << 6);
	sample_address_  = 0xc000 | (value << 6);
}

//------------------------------------------------------------------------------
// Name: write_reg3
//------------------------------------------------------------------------------
void DMC::write_reg3(uint8_t value) {
	bytes_remaining_ = (value << 4) | 1;
	sample_length_   = (value << 4) | 1;
}

//------------------------------------------------------------------------------
// Name: enabled
//------------------------------------------------------------------------------
bool DMC::enabled() const {
	return enabled_;
}

//------------------------------------------------------------------------------
// Name: sample_length
//------------------------------------------------------------------------------
uint16_t DMC::sample_length() const {
	return bytes_remaining_;
}

//------------------------------------------------------------------------------
// Name: tick
//------------------------------------------------------------------------------
void DMC::tick() {

	if(bytes_remaining_) {
		if(--frequency_ == 0) {
			frequency_ = reload_frequency_;
		
			nes::cpu.burn(4);
			sample_buffer_ = nes::cart.mapper()->read_memory(current_address_);


			if(current_address_ == 0xffff) {
				current_address_ = 0x8000;
			} else {
				++current_address_;
			}


			--bytes_remaining_;
			if(bytes_remaining_ == 0) {
				if(loop_) {
					bytes_remaining_ = sample_length_;
					current_address_ = sample_address_;
				} else if(irq_enabled_) {
					// DO IRQ
					//nes::cpu.irq(CPU::APU_IRQ);
				}
			}
		}
	}
}
