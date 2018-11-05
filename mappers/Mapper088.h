
#ifndef MAPPER088_20080314_H_
#define MAPPER088_20080314_H_

#include "Mapper.h"

class Mapper88 final : public Mapper {
public:
	Mapper88();

public:
	std::string name() const override;

public:
	void write_8(uint16_t address, uint8_t value) override;

private:
	uint8_t command_ = 0;
};

#endif
