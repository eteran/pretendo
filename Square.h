
#ifndef SQUARE_20130206_H_
#define SQUARE_20130206_H_

#include "Envelope.h"
#include "LengthCounter.h"
#include "Sweep.h"
#include "Timer.h"
#include <cstdint>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

template <int Channel>
class Square {
	friend class Sweep<Channel>;
	
public:
	Square();
	~Square();
	
private:
	Square(const Square &) = delete;
	Square &operator=(const Square &) = delete;

public:
	void enable();
	void disable();

public:
	void write_reg0(uint8_t value);
	void write_reg1(uint8_t value);
	void write_reg2(uint8_t value);
	void write_reg3(uint8_t value);

public:
	bool enabled() const;

public:
	LengthCounter  &length_counter();
	Envelope       &envelope();
	Sweep<Channel> &sweep();

public:
	void tick();
	uint8_t output() const;

private:
	LengthCounter  length_counter_;
	Envelope       envelope_;
	Timer          timer_;
	Sweep<Channel> sweep_;
	uint16_t       timer_reload_;
	uint8_t        duty_;
	uint8_t        sequence_index_;
	bool           enabled_;
};

#include "Square.tcc"

#endif
