
#ifndef TRIANGLE_20130206_H_
#define TRIANGLE_20130206_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include "LengthCounter.h"

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Triangle {
public:
	Triangle();
	~Triangle();

public:
	void reset();
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

public:
	void tick();

private:
	LengthCounter length_counter_;
	bool          enabled_;
	uint16_t      linear_counter_;
	uint16_t      linear_counter_control_;
	uint16_t      linear_counter_reload_;
	bool          linear_counter_halt_;
};

#endif
