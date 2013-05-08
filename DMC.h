
#ifndef DMC_20130206_H_
#define DMC_20130206_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include "Timer.h"

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class DMC : public boost::noncopyable {
public:
	DMC();
	~DMC();

public:
	void enable();
	void disable();

public:
	void write_reg0(uint8_t value);
	void write_reg1(uint8_t value);
	void write_reg2(uint8_t value);
	void write_reg3(uint8_t value);

public:
	uint16_t bytes_remaining() const;

public:
	void tick();
	uint8_t output() const;

private:
	bool     muted_;
	uint16_t sample_pointer_;
	uint16_t sample_address_;
	uint16_t bytes_remaining_;
	uint16_t sample_length_;
	uint8_t  irq_enabled_;
	uint8_t  loop_;
	uint8_t  sample_shift_counter_;
	uint8_t  sample_buffer_;
	Timer    timer_;
	uint8_t  output_;
};

#endif
