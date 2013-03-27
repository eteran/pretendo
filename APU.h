
#ifndef APU_20130206_H_
#define APU_20130206_H_

#include "Reset.h"
#include "Square.h"
#include "Triangle.h"
#include "Noise.h"
#include "DMC.h"

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class APU : public boost::noncopyable {
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

private:
	void clock_frame_mode_0();
	void clock_frame_mode_1();
	void clock_length();
	void clock_linear();

private:
	Square   square_1;
	Square   square_2;
	Triangle triangle_;
	Noise    noise_;
	DMC      dmc_;
	
	uint64_t apu_cycles_;
	uint64_t next_clock_;
	uint8_t  clock_step_;
	uint8_t  status_;
	uint8_t  frame_counter_;
	uint8_t  last_frame_counter_;
};

#endif
