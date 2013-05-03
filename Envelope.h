
#ifndef ENVELOPE_20130501_H_
#define ENVELOPE_20130501_H_

#include <boost/cstdint.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Envelope {
public:
	Envelope();
	~Envelope();
	
public:
	void clock();
	void start();
	void set_constant(uint8_t value);
	void set_divider(uint8_t value);
	void set_loop(uint8_t value);
	uint8_t volume() const;
	
private:
	void clock_divider();

private:
	bool    start_;
	uint8_t loop_;
	uint8_t constant_;
	uint8_t constant_volume_;
	uint8_t counter_;
	uint8_t divider_;
};

#endif
