
#ifndef MAPPER022_20080314_H_
#define MAPPER022_20080314_H_

#include "VRC2.h"

class Mapper22 final : public VRC2 {
public:
	Mapper22();

public:
	std::string name() const override;

public:
	uint8_t read_6(uint_least16_t address) override;

public:
	void write_6(uint_least16_t address, uint8_t value) override;
	void write_8(uint_least16_t address, uint8_t value) override;
	void write_9(uint_least16_t address, uint8_t value) override;
	void write_a(uint_least16_t address, uint8_t value) override;
	void write_b(uint_least16_t address, uint8_t value) override;
	void write_c(uint_least16_t address, uint8_t value) override;
	void write_d(uint_least16_t address, uint8_t value) override;
	void write_e(uint_least16_t address, uint8_t value) override;
};

#endif
