
#include "APU.h"
#include "NES.h"
#include <iostream>
#include <cassert>
#include <cstring>

namespace {

const int FrameMode        = 0x80;
const int FrameInhibitIRQ  = 0x40;
const double CPUFrequency  = 1789772.67; // 1.78977267Mhz

//------------------------------------------------------------------------------
// Name: bound
//------------------------------------------------------------------------------
template<class T>
const T &bound(const T &lower, const T &value, const T &upper) {
	using std::min;
	using std::max;

	return max(lower, min(value, upper));
}


// CPUFrequency / 44100Hz  = 40.5844142857 clocks per sample
// CPUFrequency / 48000Hz  = 37.286930625  clocks per sample
// CPUFrequency / 192000Hz = 9.32173265625 clocks per sample
const int ClocksPerSample = CPUFrequency / APU::frequency;

}

//------------------------------------------------------------------------------
// Name: APU
//------------------------------------------------------------------------------
APU::APU() : square_0_(0), square_1_(1), apu_cycles_(-1), next_clock_(-1),
		clock_step_(0), status_(0), frame_counter_(0), last_frame_counter_(0),
		sample_index_(0) {

	using std::memset;
	memset(sample_buffer_, 0, sizeof(sample_buffer_));

}

//------------------------------------------------------------------------------
// Name: ~APU
//------------------------------------------------------------------------------
APU::~APU() {

}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void APU::reset(nes::RESET reset_type) {

	status_        = 0;
	frame_counter_ = 0;
	apu_cycles_    = 0;
	next_clock_    = 0;
	clock_step_    = 0;

	if(reset_type == nes::HARD_RESET) {
		last_frame_counter_ = 0;
	}

	write4017(last_frame_counter_);
	write4015(0x00);

	// square 1
	write4000(10);
	write4001(00);
	write4002(00);
	write4003(00);

	// square 2
	write4004(10);
	write4005(00);
	write4006(00);
	write4007(00);

	// triangle
//	write4008(10); // triangle is unaffected..
	write400A(00);
	write400B(00);

	// noise
	write400C(10);
	write400E(00);
	write400F(00);

	// dmc
	write4010(10);

	// OK, the APU is supposed to act as if it has run for approximately 9
	// cycles by the time the reset is complete. I beleive that the first 7
	// of these cycles are the 7 cycles of the reset itself. So we run the
	// APU manually for an extra 2 ticks.
	//
	// Blargg says it is as if this happens
	//
	//       lda   #$00
	//       sta   $4017       ; 1
	//       lda   <0          ; 9 delay
	//       nop
	//       nop
	//       nop
	//     reset:
	if(reset_type == nes::HARD_RESET) {
		run(2);
	}

	std::cout << "[APU::reset] reset complete" << std::endl;
}

//------------------------------------------------------------------------------
// Name: write4000
//------------------------------------------------------------------------------
void APU::write4000(uint8_t value) {
	square_0_.write_reg0(value);
}

//------------------------------------------------------------------------------
// Name: write4001
//------------------------------------------------------------------------------
void APU::write4001(uint8_t value) {
	square_0_.write_reg1(value);
}

//------------------------------------------------------------------------------
// Name: write4002
//------------------------------------------------------------------------------
void APU::write4002(uint8_t value) {
	square_0_.write_reg2(value);
}

//------------------------------------------------------------------------------
// Name: write4003
//------------------------------------------------------------------------------
void APU::write4003(uint8_t value) {
	square_0_.write_reg3(value);
}

//------------------------------------------------------------------------------
// Name: write4004
//------------------------------------------------------------------------------
void APU::write4004(uint8_t value) {
	square_1_.write_reg0(value);
}

//------------------------------------------------------------------------------
// Name: write4005
//------------------------------------------------------------------------------
void APU::write4005(uint8_t value) {
	square_1_.write_reg1(value);
}

//------------------------------------------------------------------------------
// Name: write4006
//------------------------------------------------------------------------------
void APU::write4006(uint8_t value) {
	square_1_.write_reg2(value);
}

//------------------------------------------------------------------------------
// Name: write4007
//------------------------------------------------------------------------------
void APU::write4007(uint8_t value) {
	square_1_.write_reg3(value);
}

//------------------------------------------------------------------------------
// Name: write4008
//------------------------------------------------------------------------------
void APU::write4008(uint8_t value) {
	triangle_.write_reg0(value);
}

//------------------------------------------------------------------------------
// Name: write400A
//------------------------------------------------------------------------------
void APU::write400A(uint8_t value) {
	triangle_.write_reg2(value);
}

//------------------------------------------------------------------------------
// Name: write400B
//------------------------------------------------------------------------------
void APU::write400B(uint8_t value) {
	triangle_.write_reg3(value);
}

//------------------------------------------------------------------------------
// Name: write400C
//------------------------------------------------------------------------------
void APU::write400C(uint8_t value) {
	noise_.write_reg0(value);
}

//------------------------------------------------------------------------------
// Name: write400E
//------------------------------------------------------------------------------
void APU::write400E(uint8_t value) {
	noise_.write_reg2(value);
}

//------------------------------------------------------------------------------
// Name: write400F
//------------------------------------------------------------------------------
void APU::write400F(uint8_t value) {
	noise_.write_reg3(value);
}

//------------------------------------------------------------------------------
// Name: write4010
//------------------------------------------------------------------------------
void APU::write4010(uint8_t value) {
	dmc_.write_reg0(value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void APU::write4011(uint8_t value) {
	dmc_.write_reg1(value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void APU::write4012(uint8_t value) {
	dmc_.write_reg2(value);
}

//------------------------------------------------------------------------------
// Name: write4013
//------------------------------------------------------------------------------
void APU::write4013(uint8_t value) {
	dmc_.write_reg3(value);
}

//------------------------------------------------------------------------------
// Name: write4015
//------------------------------------------------------------------------------
void APU::write4015(uint8_t value) {

	// Writing to this register clears the DMC interrupt flag.
	status_ &= ~STATUS_DMC_IRQ;

	if(value & STATUS_ENABLE_SQUARE_1) {
		square_0_.enable();
	} else {
		square_0_.disable();
	}

	if(value & STATUS_ENABLE_SQUARE_2) {
		square_1_.enable();
	} else {
		square_1_.disable();
	}

	if(value & STATUS_ENABLE_TRIANGLE) {
		triangle_.enable();
	} else {
		triangle_.disable();
	}

	if(value & STATUS_ENABLE_NOISE) {
		noise_.enable();
	} else {
		noise_.disable();
	}

	if(value & STATUS_ENABLE_DMC) {
		dmc_.enable();
	} else {
		dmc_.disable();
	}

	if(!(status_ & (STATUS_DMC_IRQ | STATUS_FRAME_IRQ))) {
		nes::cpu.clear_irq(CPU::APU_IRQ);
	}
}

//------------------------------------------------------------------------------
// Name: read4015
//------------------------------------------------------------------------------
uint8_t APU::read4015() {
	uint8_t ret = status_ & (STATUS_DMC_IRQ | STATUS_FRAME_IRQ);

	// reading this register clears the Frame interrupt flag.
	status_ &= ~STATUS_FRAME_IRQ;

	if(square_0_.length_counter().value() > 0) {
		ret |= STATUS_ENABLE_SQUARE_1;
	}

	if(square_1_.length_counter().value() > 0) {
		ret |= STATUS_ENABLE_SQUARE_2;
	}

	if(triangle_.length_counter().value() > 0) {
		ret |= STATUS_ENABLE_TRIANGLE;
	}

	if(noise_.length_counter().value() > 0) {
		ret |= STATUS_ENABLE_NOISE;
	}

	if(dmc_.bytes_remaining() > 0) {
		ret |= STATUS_ENABLE_DMC;
	}

	if(!(status_ & (STATUS_DMC_IRQ | STATUS_FRAME_IRQ))) {
		nes::cpu.clear_irq(CPU::APU_IRQ);
	}

	return ret;
}

//------------------------------------------------------------------------------
// Name: write4017
//------------------------------------------------------------------------------
void APU::write4017(uint8_t value) {

	frame_counter_      = value;
	last_frame_counter_ = value;

	if(frame_counter_ & FrameInhibitIRQ) {
		status_ &= ~STATUS_FRAME_IRQ;
		if(!(status_ & (STATUS_DMC_IRQ | STATUS_FRAME_IRQ))) {
			nes::cpu.clear_irq(CPU::APU_IRQ);
		}
	}

	next_clock_ = apu_cycles_ + (apu_cycles_ & 1) + 1;

	clock_step_ = 0;

	if(!(frame_counter_ & FrameMode)) {
		next_clock_ += 7458;
	}
}

//------------------------------------------------------------------------------
// Name: clock_length
//------------------------------------------------------------------------------
void APU::clock_length() {
	square_0_.length_counter().clock();
	square_1_.length_counter().clock();
	triangle_.length_counter().clock();
	noise_.length_counter().clock();

	square_0_.sweep().clock();
	square_1_.sweep().clock();
}

//------------------------------------------------------------------------------
// Name: clock_linear
//------------------------------------------------------------------------------
void APU::clock_linear() {
	triangle_.linear_counter().clock();

	square_0_.envelope().clock();
	square_1_.envelope().clock();
	noise_.envelope().clock();
}

//------------------------------------------------------------------------------
// Name: clock_frame_mode_0
//------------------------------------------------------------------------------
void APU::clock_frame_mode_0() {

	// 4 step sequence
	switch(clock_step_) {
	case 0:
		clock_linear();
		next_clock_ += 7456;
		break;

	case 1:
		clock_linear();
		clock_length();
		next_clock_ += 7458;
		break;

	case 2:
		clock_linear();
		next_clock_ += 7457;
		break;

	case 3:
		if(!(frame_counter_ & FrameInhibitIRQ)) {
			status_ |= STATUS_FRAME_IRQ;
		}

		++next_clock_;
		break;

	case 4:
		clock_linear();
		clock_length();
		if(!(frame_counter_ & FrameInhibitIRQ)) {
			status_ |= STATUS_FRAME_IRQ;
		}

		++next_clock_;
		break;

	case 5:
		if(!(frame_counter_ & FrameInhibitIRQ)) {
			status_ |= STATUS_FRAME_IRQ;
		}

		next_clock_ += 7457;
		break;
	}

	clock_step_ = (clock_step_ + 1) % 6;
}

//------------------------------------------------------------------------------
// Name: clock_frame_mode_1
//------------------------------------------------------------------------------
void APU::clock_frame_mode_1() {

	// 5 step sequence
	switch(clock_step_) {
	case 0:
		clock_linear();
		clock_length();
		next_clock_ += 7458;
		break;

	case 1:
		clock_linear();
		next_clock_ += 7456;
		break;

	case 2:
		clock_linear();
		clock_length();
		next_clock_ += 7458;
		break;

	case 3:
		clock_linear();
		next_clock_ += 7456;
		break;

	case 4:
		next_clock_ += 7454;
		break;
	}

	clock_step_ = (clock_step_ + 1) % 5;
}

//------------------------------------------------------------------------------
// Name: run
//------------------------------------------------------------------------------
void APU::run(int cycles) {

	while(cycles-- > 0) {

		if(!(frame_counter_ & FrameInhibitIRQ) && (status_ & STATUS_FRAME_IRQ)) {
			nes::cpu.irq(CPU::APU_IRQ);
		}

		if(apu_cycles_ == next_clock_) {
			if(frame_counter_ & FrameMode) {
				clock_frame_mode_1();
			} else {
				clock_frame_mode_0();
			}
		}

		if(sample_index_ != sizeof(sample_buffer_)) {
			if((apu_cycles_ % ClocksPerSample) == 0) {
				sample_buffer_[sample_index_++] = mix_channels();
			}
		}

		dmc_.tick();
		noise_.tick();
		triangle_.tick();
		square_0_.tick();
		square_1_.tick();

		++apu_cycles_;
	}
}

//------------------------------------------------------------------------------
// Name: buffer
//------------------------------------------------------------------------------
const uint8_t *APU::buffer() {
	sample_index_ = 0;
	return sample_buffer_;
}

//------------------------------------------------------------------------------
// Name: mix_channels
//------------------------------------------------------------------------------
uint8_t APU::mix_channels() const {

	const uint8_t pulse1   = square_0_.output();
	const uint8_t pulse2   = square_1_.output();
	const uint8_t triangle = triangle_.output();
	const uint8_t noise    = noise_.output();
	const uint8_t dmc      = dmc_.output();
	
#if 0
	float pulse_volume = 0;
	float tnd_volume   = 0;

	if(pulse1 || pulse2) {
		pulse_volume = 95.88 / ((8128.0 / (pulse1 + pulse2)) + 100.0);
	}

	if(triangle || noise || dmc) {
		tnd_volume = 159.79 / ((1 / ((triangle / 8227.0) + (noise / 12241.0) + (dmc / 22638.0))) + 100.0);
	}

	int result = (pulse_volume + tnd_volume) * 0x100;
#else
	int result = (
		pulse1   +
		pulse2   +
		triangle +
		noise    +
		dmc      +
		0);
#endif

	return bound(0, result, 255);
}
