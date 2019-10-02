
#ifndef NOISE_20130206_H_
#define NOISE_20130206_H_

#include "Envelope.h"
#include "LengthCounter.h"
#include "Lfsr.h"
#include "Timer.h"
#include <cstdint>

namespace nes {
namespace apu {

using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

class Noise {
public:
	void enable();
	void disable();
	void set_enabled(bool value);

public:
	void write_reg0(uint8_t value);
	void write_reg2(uint8_t value);
	void write_reg3(uint8_t value);

public:
	bool enabled() const;

public:
	void    tick();
	uint8_t output() const;

public:
	LengthCounter length_counter;
	Envelope      envelope;

private:
	bool  enabled_ = false;
	Timer timer_;
	LFSR  lfsr_;
};

}
}

#endif
