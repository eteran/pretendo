
#ifndef MAPPER243_20080314_H_
#define MAPPER243_20080314_H_

#include "Mapper.h"

class Mapper243 : public Mapper {
public:
	Mapper243();

public:
	virtual std::string name() const;

public:
	virtual void write_4(uint16_t address, uint8_t value);

private:
	uint8_t address_;
	uint8_t regs_[8];
};

#endif
