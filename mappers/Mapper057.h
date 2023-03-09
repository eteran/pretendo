
#ifndef MAPPER057_20080314_H_
#define MAPPER057_20080314_H_

#include "Mapper.h"

class Mapper57 final : public Mapper {
public:
	Mapper57();

public:
	std::string name() const override;

public:
	void write_8(uint_least16_t address, uint8_t value) override;
	void write_9(uint_least16_t address, uint8_t value) override;
	void write_a(uint_least16_t address, uint8_t value) override;
	void write_b(uint_least16_t address, uint8_t value) override;
	void write_c(uint_least16_t address, uint8_t value) override;
	void write_d(uint_least16_t address, uint8_t value) override;
	void write_e(uint_least16_t address, uint8_t value) override;
	void write_f(uint_least16_t address, uint8_t value) override;

private:
	void write_hander(uint_least16_t address, uint8_t value);

private:
	uint8_t reg0_ = 0;
	uint8_t reg1_ = 0;
};

#endif
