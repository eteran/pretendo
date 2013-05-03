
#ifndef LENGTH_COUNTER_20130206_H_
#define LENGTH_COUNTER_20130206_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class LengthCounter : public boost::noncopyable {
public:
	LengthCounter();
	~LengthCounter();

public:
	void clear();
	void halt();
	void load(uint8_t index);
	void resume();
	void clock();

public:
	uint8_t value() const;

private:
	uint64_t        halt_cycle_;
	uint64_t        reload_cycle_;
	uint8_t         reload_value_;
	mutable uint8_t value_;
	bool            halt_;
	bool            prev_halt_;
	mutable bool    reload_;
};

#endif
