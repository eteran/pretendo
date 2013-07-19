
#ifndef DMC_20130206_H_
#define DMC_20130206_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include "Timer.h"
#include "ShiftRegister.h"

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
	void load_sample_buffer(uint8_t value);

public:
	void tick();
	uint8_t output() const;

private:
	bool irq_enabled() const;
	bool loop() const;

private:
	bool                   muted_;
	uint16_t               sample_pointer_;
	uint16_t               sample_address_;
	uint16_t               bytes_remaining_;
	uint16_t               sample_length_;
	uint8_t                bits_remaining_;
	ShiftRegister<uint8_t> sample_buffer_;
	Timer                  timer_;
	uint8_t                output_;
	uint8_t                control_;
};

#endif
