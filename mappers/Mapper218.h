
#ifndef MAPPER218_20080314_H_
#define MAPPER218_20080314_H_

#include "Mapper.h"

class Mapper218 final : public Mapper {
public:
	Mapper218();

public:
	std::string name() const override;

public:
	uint8_t read_vram(uint_least16_t address) override;
	void write_vram(uint_least16_t address, uint8_t value) override;
};

#endif
