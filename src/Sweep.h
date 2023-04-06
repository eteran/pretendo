
#ifndef SWEEP_20130502_H_
#define SWEEP_20130502_H_

#include <cstdint>

namespace nes::apu {

template <int Channel>
class Square;

template <int Channel>
class Sweep {
public:
	explicit Sweep(Square<Channel> *square)
		: square_(square) {
	}

	~Sweep()             = default;
	Sweep(const Sweep &) = delete;
	Sweep &operator=(const Sweep &) = delete;

public:
	void clock() {
		// If the reload flag flag is set, the divider's counter is set to the
		// period P. If the divider's counter was zero before the reload, the
		// pulse's period is also adjusted.
		if (reload_) {
			reload_                    = false;
			const uint8_t prev_counter = counter_;
			counter_                   = period() + 1;

			if (prev_counter == 0 && enabled() && shift()) {
				const uint16_t target = target_period();
				if (target < 0x800) {
					square_->timer_reload_    = target;
					pulse_period_             = target;
					square_->timer_.frequency = (target + 1) * 2;
				}
			}

		} else {
			if (counter_) {
				// If the reload flag is clear and the divider's counter is non-zero,
				// it is decremented.
				--counter_;
			} else {
				// If the reload flag is clear and the divider's counter is zero,
				// the counter is set to P and the pulse's period is adjusted.
				counter_ = period() + 1;

				if (enabled() && shift()) {
					const uint16_t target = target_period();
					if (target < 0x800) {
						square_->timer_reload_    = target;
						pulse_period_             = target;
						square_->timer_.frequency = (target + 1) * 2;
					}
				}
			}
		}
	}

	void set_control(uint8_t value) {
		control_ = value;
		reload_  = true;
	}

	void set_pulse_period(uint16_t value) {
		pulse_period_ = value;
	}

	bool silenced() const {
		return silenced_;
	}

private:
	bool enabled() const {
		return control_ & 0x80;
	}

	bool negate() const {
		return control_ & 0x08;
	}

	uint16_t target_period() const {
		// The channel's 11-bit raw timer period is shifted right by the shift count
		// (using a barrel shifter), then either added to or subtracted from the
		// channel's raw period, yielding the target period.
		const uint16_t delta = pulse_period_ >> shift();

		if (negate()) {
			if constexpr (Channel == 0) {
				return pulse_period_ - delta - 1;
			} else {
				return pulse_period_ - delta;
			}
		} else {
			return pulse_period_ + delta;
		}
	}

	uint8_t period() const {
		return (control_ >> 4) & 0x07;
	}

	uint8_t shift() const {
		return control_ & 0x07;
	}

private:
	Square<Channel> *square_;
	uint16_t pulse_period_ = 0;
	uint8_t counter_       = 0;
	uint8_t control_       = 0;
	bool reload_           = false;
	bool silenced_         = false;
};

}

#endif
