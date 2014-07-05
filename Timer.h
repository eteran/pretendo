
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
	void reset();
	uint16_t value() const;

public:
	template <class Callback>
	void tick(Callback c) {
		if(--timer_ == 0) {
			timer_ = frequency;
			c();
		}
	}

public:
	uint16_t frequency;

private:
	uint16_t timer_;
};

#endif

