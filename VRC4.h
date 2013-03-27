
#ifndef VRC4_20120121_H_
#define VRC4_20120121_H_

#include "Mapper.h"

class VRC4 : public Mapper {
public:
	VRC4();

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
	uint8_t irq_latch_;
	uint8_t irq_control_;
	uint8_t irq_counter_;
	int     irq_prescaler_;

	uint8_t chr_[8];
	uint8_t prg_[2];
	uint8_t prg_mode_;
};

#endif
