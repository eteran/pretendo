
#ifndef SWEEP_20130502_H_
#define SWEEP_20130502_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Square;

class Sweep : public boost::noncopyable {
public:
	Sweep(int channel, Square *square);
	~Sweep();

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
	Square *const square_;
	int           channel_;
	uint16_t      pulse_period_;
	uint8_t       counter_;
	uint8_t       control_;
	bool          reload_;
	bool          silenced_;
};

#endif

