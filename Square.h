
#ifndef SQUARE_20130206_H_
#define SQUARE_20130206_H_

#include "Envelope.h"
#include "LengthCounter.h"
#include "Sweep.h"
#include "Timer.h"
#include <cstdint>

namespace nes {
namespace apu {

using std::uint8_t;
using std::uint16_t;

template <int Channel>
class Square {
	friend class Sweep<Channel>;

public:
	Square();
	~Square() = default;

private:
	Square(const Square &) = delete;
	Square &operator=(const Square &) = delete;

public:
	void enable();
	void disable();
	void set_enabled(bool value);

public:
	void write_reg0(uint8_t value);
	void write_reg1(uint8_t value);
	void write_reg2(uint8_t value);
	void write_reg3(uint8_t value);

public:
	bool enabled() const;

public:
	void tick();
	uint8_t output() const;

public:
	LengthCounter  length_counter;
	Envelope       envelope;
	Sweep<Channel> sweep;

private:
	Timer          timer_;
	uint16_t       timer_reload_;
	uint8_t        duty_;
	uint8_t        sequence_index_;
	bool           enabled_;
};

}
}

#include "Square.tcc"

#endif
