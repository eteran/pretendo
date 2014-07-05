
#include "DMC.h"
#include "APU.h"
#include "CPU.h"
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
// Name: DMC
//------------------------------------------------------------------------------
DMC::DMC() : muted_(false), sample_pointer_(0xc000), sample_address_(0xc000), bytes_remaining_(0), sample_length_(0), bits_remaining_(0), control_(0) {
}

//------------------------------------------------------------------------------
// Name: ~DMC
//------------------------------------------------------------------------------
DMC::~DMC() {
}

//------------------------------------------------------------------------------
// Name: load_sample_buffer
//------------------------------------------------------------------------------
void DMC::load_sample_buffer(uint8_t value) {
	sample_buffer_.load(value);
}

//------------------------------------------------------------------------------
// Name: enable
//------------------------------------------------------------------------------
void DMC::enable() {

	if(bytes_remaining_ == 0) {
		bytes_remaining_ = sample_length_;
	}

	if(bits_remaining_ != 0) {
		if(sample_buffer_.read()) {
			if(output_ < 0x7e) {
				output_ += 2;
			}
		} else {
			if(output_ > 0x01) {
				output_ -= 2;
			}
		}
		--bits_remaining_;
	}

	// immediate load the first byte if the shift counter is empty
	if(bytes_remaining_ != 0 && bits_remaining_ == 0) {
		bits_remaining_ = 8;

		nes::cpu::schedule_dmc_dma([](uint8_t value){
			nes::apu::dmc.load_sample_buffer(value);
		}, sample_pointer_, 1);

		sample_pointer_ = ((sample_pointer_ + 1) & 0xffff) | 0x8000;

		if(--bytes_remaining_ == 0) {
			if(loop()) {
				bytes_remaining_ = sample_length_;
				sample_pointer_  = sample_address_;
			} else if(irq_enabled()) {
				nes::cpu::irq(nes::cpu::APU_IRQ);
				nes::apu::status.dmc_irq = true;
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void DMC::disable() {

	bytes_remaining_ = 0;
}

//------------------------------------------------------------------------------
// Name: write_reg0
//------------------------------------------------------------------------------
void DMC::write_reg0(uint8_t value) {

	control_ = value;

	timer_.frequency = frequency_table[control_ & 0x0f];
	timer_.reset();

	if(!irq_enabled()) {
		nes::apu::status.dmc_irq = false;
		if(!nes::apu::status.irq_firing) {
			nes::cpu::clear_irq(nes::cpu::APU_IRQ);
		}
	}
}

//------------------------------------------------------------------------------
// Name: write_reg1
//------------------------------------------------------------------------------
void DMC::write_reg1(uint8_t value) {
	output_ = value & 0x7f;
}

//------------------------------------------------------------------------------
// Name: write_reg2
//------------------------------------------------------------------------------
void DMC::write_reg2(uint8_t value) {
	sample_address_ = 0xc000 | (value << 6);
	sample_pointer_ = sample_address_;
}

//------------------------------------------------------------------------------
// Name: write_reg3
//------------------------------------------------------------------------------
void DMC::write_reg3(uint8_t value) {
	sample_length_ = (value << 4) | 1;
	if(bytes_remaining_ == 0) {
		bytes_remaining_ = sample_length_;
	}
}

//------------------------------------------------------------------------------
// Name: bytes_remaining
//------------------------------------------------------------------------------
uint16_t DMC::bytes_remaining() const {
	return bytes_remaining_;
}

//------------------------------------------------------------------------------
// Name: tick
//------------------------------------------------------------------------------
void DMC::tick() {


	timer_.tick([this]() {

		if(bits_remaining_ != 0) {
			if(sample_buffer_.read()) {
				if(output_ < 0x7e) {
					output_ += 2;
				}
			} else {
				if(output_ > 0x01) {
					output_ -= 2;
				}
			}
			--bits_remaining_;
		}

		if(bytes_remaining_ != 0 && bits_remaining_ == 0) {
			bits_remaining_ = 8;

			nes::cpu::schedule_dmc_dma([](uint8_t value){
				nes::apu::dmc.load_sample_buffer(value);
			}, sample_pointer_, 1);

			sample_pointer_ = ((sample_pointer_ + 1) & 0xffff) | 0x8000;

			if(--bytes_remaining_ == 0) {
				if(loop()) {
					bytes_remaining_ = sample_length_;
					sample_pointer_  = sample_address_;
				} else if(irq_enabled()) {
					nes::cpu::irq(nes::cpu::APU_IRQ);
					nes::apu::status.dmc_irq = true;
				}
			}
		}
	});
}

//------------------------------------------------------------------------------
// Name: output
//------------------------------------------------------------------------------
uint8_t DMC::output() const {
	return output_ & 0x7f;
}

//------------------------------------------------------------------------------
// Name: irq_enabled
//------------------------------------------------------------------------------
bool DMC::irq_enabled() const {
	return control_ & 0x80;
}

//------------------------------------------------------------------------------
// Name: loop
//------------------------------------------------------------------------------
bool DMC::loop() const {
	return control_ & 0x40;
}
