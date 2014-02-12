
#ifndef TRIANGLE_20130206_H_
#define TRIANGLE_20130206_H_

#include <cstdint>
#include <cstddef>
#include <boost/noncopyable.hpp>
#include "LengthCounter.h"
#include "LinearCounter.h"
#include "Timer.h"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

class Triangle : public boost::noncopyable {
public:
	Triangle();
	~Triangle();

public:
	void enable();
	void disable();

public:
	void write_reg0(uint8_t value);
	void write_reg2(uint8_t value);
	void write_reg3(uint8_t value);

public:
	bool enabled() const;

public:
	LengthCounter &length_counter();
	LinearCounter &linear_counter();

public:
	void tick();
	uint8_t output() const;

private:
	LengthCounter length_counter_;
	LinearCounter linear_counter_;
	bool          enabled_;
	uint16_t      timer_load_;
	size_t        sequence_index_;
	Timer         timer_;
};

#endif
