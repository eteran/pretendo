
#ifndef TIMER_20130424_H_
#define TIMER_20130424_H_

#include <cstdint>

namespace nes {
namespace apu {

using std::uint16_t;

class Timer {
public:
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
	uint16_t frequency = 0xffff;

private:
	uint16_t timer_ = 0xffff;
};

}
}

#endif

