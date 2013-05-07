
#ifndef ENVELOPE_20130501_H_
#define ENVELOPE_20130501_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Envelope : public boost::noncopyable {
public:
	Envelope();
	~Envelope();

public:
	void clock();
	void start();
	void set_control(uint8_t value);
	uint8_t volume() const;

private:
	void clock_divider();

private:
	bool    start_;
	uint8_t counter_;
	uint8_t divider_;
	uint8_t control_;
};

#endif
