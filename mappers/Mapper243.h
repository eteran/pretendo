
#ifndef MAPPER243_20080314_H_
#define MAPPER243_20080314_H_

#include "Mapper.h"

class Mapper243 final : public Mapper {
public:
	Mapper243();

public:
	std::string name() const override;

public:
	void write_4(uint16_t address, uint8_t value) override;

private:
	uint8_t regs_[8] = {};
	uint8_t address_ = 0;
};

#endif
