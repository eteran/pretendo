
#ifndef MAPPER232_20080314_H_
#define MAPPER232_20080314_H_

#include "Mapper.h"

class Mapper232 final : public Mapper {
public:
	Mapper232();

public:
	std::string name() const override;

	void write_8(uint_least16_t address, uint8_t value) override;
	void write_9(uint_least16_t address, uint8_t value) override;
	void write_c(uint_least16_t address, uint8_t value) override;
	void write_d(uint_least16_t address, uint8_t value) override;
	void write_e(uint_least16_t address, uint8_t value) override;
	void write_f(uint_least16_t address, uint8_t value) override;

private:
	uint8_t chr_ram_[0x2000] = {};
};

#endif
