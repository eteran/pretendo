
#ifndef ENVELOPE_20130501_H_
#define ENVELOPE_20130501_H_

#include <cstdint>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

class Envelope {
public:
	Envelope();
	~Envelope();
	
private:
	Envelope(const Envelope &) = delete;
	Envelope &operator=(const Envelope &) = delete;

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
