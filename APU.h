
#ifndef APU_20130206_H_
#define APU_20130206_H_

#include "Reset.h"
#include "BitField.h"
#include <cstdint>
#include <cstddef>

namespace nes {
namespace apu {

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

template <int Channel>
class Square;
class Triangle;
class Noise;
class DMC;

union APUStatus {
	uint8_t raw;
	BitField<0> square1_enabled;
	BitField<1> square2_enabled;
	BitField<2> triangle_enabled;
	BitField<3> noise_enabled;
	BitField<4> dmc_enabled;
	BitField<6> frame_irq;
	BitField<7> dmc_irq;

	// meta field
	BitField<6,2> irq_firing;
};

static const int frequency   = 44100;
static const int buffer_size = frequency / 60;

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
void run(int cycles);
const uint8_t *buffer();

extern Square<0> square_0;
extern Square<1> square_1;
extern Triangle  triangle;
extern Noise     noise;
extern DMC       dmc;

extern APUStatus status;

}
}

#endif
