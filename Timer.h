
#ifndef TIMER_20130424_H_
#define TIMER_20130424_H_

#include <cstdint>

using std::uint8_t;
using std::uint16_t;

class Timer {
public:
	Timer();
	
private:
	Timer(const Timer &) = delete;
	Timer &operator=(const Timer &) = delete;

public:
	bool tick();
	void set_frequency(uint16_t frequency);
	void reset();
	uint16_t frequency() const;
	uint16_t value() const;

private:
	uint16_t timer_;
	uint16_t frequency_;
};

#endif

