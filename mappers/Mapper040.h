
#ifndef MAPPER040_20080314_H_
#define MAPPER040_20080314_H_

#include "Mapper.h"

class Mapper40 final : public Mapper {
public:
	Mapper40();

public:
	std::string name() const override;

	void write_8(uint_least16_t address, uint8_t value) override;
	void write_9(uint_least16_t address, uint8_t value) override;
	void write_a(uint_least16_t address, uint8_t value) override;
	void write_b(uint_least16_t address, uint8_t value) override;
	void write_e(uint_least16_t address, uint8_t value) override;
	void write_f(uint_least16_t address, uint8_t value) override;

public:
	void cpu_sync() override;

private:
	uint_least16_t irq_counter_ = 0;
	bool irq_enabled_     = false;
};

#endif
