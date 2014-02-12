
#ifndef APU_20130206_H_
#define APU_20130206_H_

#include "Reset.h"
#include "Square.h"
#include "Triangle.h"
#include "Noise.h"
#include "DMC.h"

#include <cstdint>
#include <cstddef>
#include <boost/noncopyable.hpp>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

class APU : public boost::noncopyable {
	friend class DMC;
	friend class Noise;
	friend class Triangle;
	friend class Square;

public:
	enum {
		STATUS_DMC_IRQ  	   = 0x80,
		STATUS_FRAME_IRQ	   = 0x40,
		STATUS_ENABLE_DMC	   = 0x10,
		STATUS_ENABLE_NOISE    = 0x08,
		STATUS_ENABLE_TRIANGLE = 0x04,
		STATUS_ENABLE_SQUARE_2 = 0x02,
		STATUS_ENABLE_SQUARE_1 = 0x01,
		STATUS_ENABLE_ALL	   = STATUS_ENABLE_SQUARE_1 | STATUS_ENABLE_SQUARE_2 | STATUS_ENABLE_TRIANGLE | STATUS_ENABLE_NOISE | STATUS_ENABLE_DMC
	};

	static const int frequency         = 48000;
	static const int buffer_size       = frequency / 60;

public:
	APU();
	~APU();

public:
	void reset(nes::RESET reset_type);

public:
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

public:
	uint8_t read4015();
	uint64_t cycle_count() const {
		return apu_cycles_;
	}

public:
	void run(int cycles);

public:
	const uint8_t *buffer();
	
public:
	Square   &square_0() { return square_0_; }
	Square   &square_1() { return square_1_; }
	Triangle &triangle() { return triangle_; }
	Noise    &noise()    { return noise_; }
	DMC      &dmc()      { return dmc_; }

private:
	void clock_frame_mode_0();
	void clock_frame_mode_1();
	void clock_length();
	void clock_linear();
	uint8_t mix_channels() const;

private:
	Square   square_0_;
	Square   square_1_;
	Triangle triangle_;
	Noise    noise_;
	DMC      dmc_;

	uint64_t apu_cycles_;
	uint64_t next_clock_;
	uint8_t  clock_step_;
	uint8_t  status_;
	uint8_t  frame_counter_;
	uint8_t  last_frame_counter_;

private:
	uint8_t sample_buffer_[buffer_size];
	size_t  sample_index_;
};

#endif
