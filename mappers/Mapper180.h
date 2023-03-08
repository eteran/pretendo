
#ifndef MAPPER180_20080314_H_
#define MAPPER180_20080314_H_

#include "Mapper.h"

class Mapper180 final : public Mapper {
public:
	Mapper180();

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

private:
	uint8_t chr_ram_[0x2000] = {};
};

#endif
