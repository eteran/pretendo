
#ifndef SQUARE_20130206_H_
#define SQUARE_20130206_H_

#include <cstdint>
#include <boost/noncopyable.hpp>
#include "LengthCounter.h"
#include "Envelope.h"
#include "Sweep.h"
#include "Timer.h"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

class Square : public boost::noncopyable {
	friend class Sweep;
	
public:
	explicit Square(int channel);
	~Square();

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
	LengthCounter &length_counter();
	Envelope      &envelope();
	Sweep         &sweep();

public:
	void tick();
	uint8_t output() const;

private:
	LengthCounter length_counter_;
	Envelope      envelope_;
	Timer         timer_;
	Sweep         sweep_;
	uint16_t      timer_reload_;
	uint8_t       duty_;
	uint8_t       sequence_index_;
	bool          enabled_;
};

#endif
