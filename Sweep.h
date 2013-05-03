
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
	void clock();
	void reload();
	void set_control(uint8_t value);
	void update_target_period(bool from_divider);
	bool silence_channel() const;
	
private:
	void clock_divider();
	
private:
	Timer   &timer_;
	uint16_t value_;
	uint16_t target_period_;
	uint8_t divider_;
	uint8_t divider_period_;
	uint8_t shift_;
	uint8_t negate_;
	uint8_t enable_;
	bool    reload_;
	bool    silence_;
};

#endif

