
#ifndef TIMER_20130424_H_
#define TIMER_20130424_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

using boost::uint8_t;
using boost::uint16_t;

class Timer : public boost::noncopyable {
public:
	Timer();

public:
	bool tick();
	void set_frequency(uint16_t frequency);
	uint16_t frequency() const;
	uint16_t value() const;

private:
	uint16_t timer_;
	uint16_t frequency_;
};

#endif

