
#ifndef MAPPER041_20080314_H_
#define MAPPER041_20080314_H_

#include "Mapper.h"

class Mapper41 final : public Mapper {
public:
	Mapper41();

public:
	std::string name() const override;

public:
	void write_6(uint_least16_t address, uint8_t value) override;
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
	uint8_t register1_ = 0;
	uint8_t register2_ = 0;
};

#endif
