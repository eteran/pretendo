
#ifndef SQUARE_20130206_H_
#define SQUARE_20130206_H_

#include "Envelope.h"
#include "LengthCounter.h"
#include "Sweep.h"
#include "Timer.h"
#include <cstdint>

namespace nes {
namespace apu {

template <int Channel>
class Square {
	friend class Sweep<Channel>;
	static_assert(Channel >= 0 && Channel < 2, "only channels 0 and 1 are valid");

public:
	void enable() {
		enabled_ = true;
	}

	void disable() {
		enabled_ = false;
		length_counter.clear();
	}

	void set_enabled(bool value) {
		if (value) {
			enable();
		} else {
			disable();
		}
	}

public:
	void write_reg0(uint8_t value) {
		duty_ = (value >> 6) & 0x03;

		if (value & 0x20) {
			length_counter.halt();
		} else {
			length_counter.resume();
		}

		envelope.set_control(value);
	}

	void write_reg1(uint8_t value) {
		sweep.set_control(value);
	}

	void write_reg2(uint8_t value) {
		timer_reload_    = (timer_reload_ & 0xff00) | value;
		timer_.frequency = (timer_reload_ + 1) * 2;
		sweep.set_pulse_period(timer_reload_);
	}

	void write_reg3(uint8_t value) {
		timer_reload_    = (timer_reload_ & 0x00ff) | ((value & 0x07) << 8);
		timer_.frequency = (timer_reload_ + 1) * 2;
		sweep.set_pulse_period(timer_reload_);

		if (enabled_) {
			length_counter.load((value >> 3) & 0x1f);
		}

		sequence_index_ = 0;
		envelope.start();
	}

public:
	bool enabled() const {
		return enabled_;
	}

public:
	void tick() {
		timer_.tick([this]() {
			sequence_index_ = (sequence_index_ + 1) % 8;
		});
	}

	uint8_t output() const {
		static const uint8_t sequence[4][8] = {
			{0, 1, 0, 0, 0, 0, 0, 0},
			{0, 1, 1, 0, 0, 0, 0, 0},
			{0, 1, 1, 1, 1, 0, 0, 0},
			{1, 0, 0, 1, 1, 1, 1, 1},
		};

		if ((timer_.frequency - 1) < 8) {
			return 0;
		} else if (sequence[duty_][sequence_index_] == 0) {
			return 0;
		} else if (sweep.silenced()) {
			return 0;
		} else if (length_counter.value() == 0) {
			return 0;
		} else {
			return envelope.volume();
		}
	}

public:
	LengthCounter length_counter;
	Envelope envelope;
	Sweep<Channel> sweep{this};

private:
	Timer timer_;
	uint16_t timer_reload_  = 0;
	uint8_t duty_           = 0;
	uint8_t sequence_index_ = 0;
	bool enabled_           = false;
};

}
}

#endif
