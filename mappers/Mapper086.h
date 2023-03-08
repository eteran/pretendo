
#ifndef MAPPER086_20080314_H_
#define MAPPER086_20080314_H_

#include "Mapper.h"

class Mapper86 final : public Mapper {
public:
	Mapper86();

public:
	std::string name() const override;

public:
	void write_6(uint32_t address, uint8_t value) override;
	void write_7(uint32_t address, uint8_t value) override;
};

#endif
