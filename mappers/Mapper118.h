
#ifndef MAPPER118_20080314_H_
#define MAPPER118_20080314_H_

#include "MMC3.h"

class Mapper118 final : public MMC3 {
public:
	Mapper118();

public:
	std::string name() const override;

public:
	void write_8(uint_least16_t address, uint8_t value) override;
	void write_9(uint_least16_t address, uint8_t value) override;
	void write_a(uint_least16_t address, uint8_t value) override;
	void write_b(uint_least16_t address, uint8_t value) override;
};

#endif
