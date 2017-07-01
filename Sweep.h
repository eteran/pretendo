
#ifndef SWEEP_20130502_H_
#define SWEEP_20130502_H_

#include <cstdint>

namespace nes {
namespace apu {

using std::uint8_t;
using std::uint16_t;

template <int Channel>
class Square;

template <int Channel>
class Sweep {
public:
	explicit Sweep(Square<Channel> *square);
	~Sweep() = default;

private:
	Sweep(const Sweep &) = delete;
	Sweep& operator=(const Sweep &) = delete;

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
	uint16_t      pulse_period_;
	uint8_t       counter_;
	uint8_t       control_;
	bool          reload_;
	bool          silenced_;
};

}
}

#include "Sweep.tcc"

#endif

