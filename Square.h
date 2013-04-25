
#ifndef SQUARE_20130206_H_
#define SQUARE_20130206_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include "LengthCounter.h"

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Square : public boost::noncopyable {
public:
	Square();
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

public:
	void tick();

private:
	LengthCounter length_counter_;
	uint16_t      timer_reload_;
	uint16_t      timer_;
	uint8_t       duty_;
	uint8_t       envelope_;
	uint8_t       sweep_enable_;
	uint8_t       sweep_period_;
	uint8_t       sweep_negate_;
	uint8_t       sweep_shift_;
	bool          enabled_;
};

#endif
