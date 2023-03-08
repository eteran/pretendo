
#ifndef VRC4_20120121_H_
#define VRC4_20120121_H_

#include "BitField.h"
#include "Mapper.h"

class VRC4 : public Mapper {
private:
	union IRQControl {
		uint8_t raw;
		BitField<uint8_t, 0> a;
		BitField<uint8_t, 1> enabled;
		BitField<uint8_t, 2> mode;
	};

public:
	VRC4();

public:
	std::string name() const override;

public:
	void write_8(uint32_t address, uint8_t value) override;
	void write_9(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_b(uint32_t address, uint8_t value) override;
	void write_c(uint32_t address, uint8_t value) override;
	void write_d(uint32_t address, uint8_t value) override;
	void write_e(uint32_t address, uint8_t value) override;
	void write_f(uint32_t address, uint8_t value) override;

public:
	void cpu_sync() override;

private:
	void clock_irq();

private:
	uint8_t chr_[8] = {};
	uint8_t prg_[2] = {};

	uint8_t irq_latch_      = 0;
	IRQControl irq_control_ = {0};
	uint8_t irq_counter_    = 0;
	uint8_t prg_mode_       = 0;
	int irq_prescaler_      = 341;
};

#endif
