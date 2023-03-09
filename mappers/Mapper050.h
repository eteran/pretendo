
#ifndef MAPPER050_20080314_H_
#define MAPPER050_20080314_H_

#include "Mapper.h"

class Mapper50 final : public Mapper {
public:
	Mapper50();

public:
	std::string name() const override;

	void write_4(uint_least16_t address, uint8_t value) override;
	void write_5(uint_least16_t address, uint8_t value) override;

public:
	void cpu_sync() override;

private:
	void write_handler(uint_least16_t address, uint8_t value);

private:
	uint8_t chr_ram_[0x2000] = {};
	uint_least16_t irq_counter_ = 0;
	bool irq_enabled_        = false;
};

#endif
