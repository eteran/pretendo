
#ifndef MAPPER015_20080314_H_
#define MAPPER015_20080314_H_

#include "Mapper.h"

class Mapper15 final : public Mapper {
public:
	Mapper15();

public:
	std::string name() const override;

	void write_8(uint_least16_t address, uint8_t value) override;
	void write_9(uint_least16_t address, uint8_t value) override;
	void write_a(uint_least16_t address, uint8_t value) override;
	void write_b(uint_least16_t address, uint8_t value) override;
	void write_c(uint_least16_t address, uint8_t value) override;
	void write_d(uint_least16_t address, uint8_t value) override;
	void write_e(uint_least16_t address, uint8_t value) override;
	void write_f(uint_least16_t address, uint8_t value) override;

private:
	void write_handler(uint_least16_t address, uint8_t value);

private:
	uint8_t chr_ram_[0x2000] = {};
};

#endif
