
#ifndef APU_20130206_H_
#define APU_20130206_H_

#include "BitField.h"
#include "Reset.h"
#include "circular_buffer.h"
#include <cstddef>
#include <cstdint>

namespace nes::apu {

template <int Channel>
class Square;
class Triangle;
class Noise;
class DMC;

union APUStatus {
	uint8_t raw;
	BitField<uint8_t, 0> square1_enabled;
	BitField<uint8_t, 1> square2_enabled;
	BitField<uint8_t, 2> triangle_enabled;
	BitField<uint8_t, 3> noise_enabled;
	BitField<uint8_t, 4> dmc_enabled;
	BitField<uint8_t, 6> frame_irq;
	BitField<uint8_t, 7> dmc_irq;

	// meta field
	BitField<uint8_t, 6, 2> irq_firing;
};

constexpr int frequency   = 44100;
constexpr int buffer_size = (frequency / 60) * 2;

void reset(Reset reset_type);

void write4000(uint8_t value);
void write4001(uint8_t value);
void write4002(uint8_t value);
void write4003(uint8_t value);
void write4004(uint8_t value);
void write4005(uint8_t value);
void write4006(uint8_t value);
void write4007(uint8_t value);
void write4008(uint8_t value);
void write400A(uint8_t value);
void write400B(uint8_t value);
void write400C(uint8_t value);
void write400E(uint8_t value);
void write400F(uint8_t value);
void write4010(uint8_t value);
void write4011(uint8_t value);
void write4012(uint8_t value);
void write4013(uint8_t value);
void write4015(uint8_t value);
void write4017(uint8_t value);

uint8_t read4015();
uint64_t cycle_count();
void tick();

template <int Cycles>
void exec() {
	for (int i = 0; i < Cycles; ++i) {
		tick();
	}
}

extern Square<0> square_0;
extern Square<1> square_1;
extern Triangle triangle;
extern Noise noise;
extern DMC dmc;

extern circular_buffer<uint8_t, buffer_size> sample_buffer_;

extern APUStatus status;

}


#endif
