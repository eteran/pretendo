
#include "DMC.h"
#include "NES.h"
#include "APU.h"
#include "Mapper.h"

namespace {

// NTSC period table
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
DMC::DMC() : enabled_(false), current_address_(0xc000), sample_address_(0xc000), bytes_remaining_(0), sample_length_(0), irq_enabled_(0x80), load_counter_(0), loop_(0), bit_(0) {
	timer_.set_frequency(0xffff);
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
	
	if(sample_length() == 0) {
		bytes_remaining_ = sample_length_;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void DMC::disable() {
	enabled_         = false;
	bytes_remaining_ = 0;
	bit_             = 0;
}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void DMC::reset() {
	enabled_          = false;
	current_address_  = 0;
	sample_address_   = 0;
	bytes_remaining_  = 0;
	sample_length_    = 0;
	irq_enabled_      = 0x80;
	load_counter_     = 0;
	loop_             = 0;
	bit_              = 0;
	
	timer_.set_frequency(0xffff);
}

//------------------------------------------------------------------------------
// Name: write_reg0
//------------------------------------------------------------------------------
void DMC::write_reg0(uint8_t value) {
	irq_enabled_      = value & 0x80;
	loop_             = value & 0x40;
	
	timer_.set_frequency(frequency_table[value & 0x0f]);

	if(!irq_enabled_) {
		nes::apu.status_ &= ~APU::STATUS_DMC_IRQ;
		if(!(nes::apu.status_ & (APU::STATUS_DMC_IRQ | APU::STATUS_FRAME_IRQ))) {
			nes::cpu.clear_irq(CPU::APU_IRQ);
		}
	}
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
	sample_length_ = (value << 4) | 1;
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
	if(enabled()) {
		if(timer_.tick()) {
			if(bytes_remaining_ != 0) {

				// read the 8-bit sample
				if(bit_ == 0) {

					// TODO: hijack the CPU for appropriate number of cycles, 
					//       not hardcoded to 4
					nes::cpu.burn(4);
					sample_buffer_ = nes::cpu.read(current_address_);
					
					
					current_address_ = ((current_address_ + 1) & 0xffff) | 0x8000;
										
					if(--bytes_remaining_ == 0) {
						if(loop_) {
							bytes_remaining_ = sample_length_;
							current_address_ = sample_address_;
						} else if(irq_enabled_) {
							nes::cpu.irq(CPU::APU_IRQ);
							nes::apu.status_ |= APU::STATUS_DMC_IRQ;
							printf("DMC IRQ\n");
						}
					}
				}

				// TODO: grab appropriate bit from sample_buffer and update
				bit_ = (bit_ + 1) % 8;
			}
		}
	}
}
