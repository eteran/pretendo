
#ifndef SWEEP_20130502_H_
#define SWEEP_20130502_H_

#include <cstdint>

namespace nes {
namespace apu {

using std::uint16_t;
using std::uint8_t;

template <int Channel>
class Square;

template <int Channel>
class Sweep {
public:
	explicit Sweep(Square<Channel> *square);
	~Sweep()             = default;
	Sweep(const Sweep &) = delete;
	Sweep &operator=(const Sweep &) = delete;

public:
	void clock();
	void set_control(uint8_t value);
	void set_pulse_period(uint16_t value);
	bool silenced() const;

private:
	bool enabled() const;
	bool negate() const;
	uint16_t target_period() const;
	uint8_t period() const;
	uint8_t shift() const;

private:
	Square<Channel> *const square_;
	uint16_t pulse_period_ = 0;
	uint8_t counter_       = 0;
	uint8_t control_       = 0;
	bool reload_           = false;
	bool silenced_         = false;
};

}
}

#include "Sweep.tcc"

#endif
