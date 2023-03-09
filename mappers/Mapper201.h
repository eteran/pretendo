
#ifndef MAPPER201_20080314_H_
#define MAPPER201_20080314_H_

#include "Mapper.h"

class Mapper201 final : public Mapper {
public:
	Mapper201();

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
	void write_handler(uint_least16_t address, uint8_t value);
};

#endif
