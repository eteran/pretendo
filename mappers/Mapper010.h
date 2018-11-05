
#ifndef MAPPER0_10_20080314_H_
#define MAPPER0_10_20080314_H_

#include "Mapper.h"

class Mapper10 final : public Mapper {
public:
	Mapper10();

public:
	std::string name() const override;

public:
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;
	void write_f(uint16_t address, uint8_t value) override;

public:
	uint8_t read_vram(uint16_t address) override;

private:
	bool    latch1_    = true;
	bool    latch2_    = true;
	uint8_t latch1_lo_ = 0;
	uint8_t latch1_hi_ = 0;
	uint8_t latch2_lo_ = 0;
	uint8_t latch2_hi_ = 0;
};

#endif
