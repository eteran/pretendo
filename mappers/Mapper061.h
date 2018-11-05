
#ifndef MAPPER061_20080314_H_
#define MAPPER061_20080314_H_

#include "Mapper.h"

class Mapper61 final : public Mapper {
public:
	Mapper61();

public:
	std::string name() const override;

public:
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;
	void write_f(uint16_t address, uint8_t value) override;

private:
	void write_hander(uint16_t address, uint8_t value);

private:
	uint8_t chr_ram_[0x2000];
};

#endif
