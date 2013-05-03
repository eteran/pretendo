
#ifndef SWEEP_20130502_H_
#define SWEEP_20130502_H_

#include <boost/cstdint.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Timer;

class Sweep {
public:
	Sweep(Timer &timer);
	~Sweep();
	
public:
	void clock(int channel);
	void reload();
	void set_control(uint8_t value);
	void set_pulse_period(uint16_t value);
	bool silenced() const;
	
private:
	uint16_t target_period(int channel) const;
	
private:
	Timer   &timer_;
	uint16_t value_;
	uint16_t pulse_period_;
	uint8_t counter_;
	uint8_t period_;
	uint8_t shift_;
	uint8_t negate_;
	uint8_t enable_;
	bool    reload_;
	bool    silenced_;
};

#endif

