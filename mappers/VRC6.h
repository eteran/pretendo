
#ifndef VRC6_20120121_H_
#define VRC6_20120121_H_

#include "Mapper.h"
#include "BitField.h"

class VRC6 : public Mapper {
private:
	union IRQControl {
		uint8_t raw;
		BitField<0> a;
		BitField<1> enabled;
		BitField<2> mode;
	};
public:
	VRC6();

public:
	virtual std::string name() const;

public:
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

public:
	virtual void cpu_sync();

private:
	void clock_irq();

private:
	uint8_t    irq_latch_;
	IRQControl irq_control_;
	uint8_t    irq_counter_;
	int        irq_prescaler_;
};

#endif
