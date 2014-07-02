
#ifndef MAPPER088_20080314_H_
#define MAPPER088_20080314_H_

#include "Mapper.h"

class Mapper88 : public Mapper {
public:
	Mapper88();

public:
	virtual std::string name() const;

public:
	virtual void write_8(uint16_t address, uint8_t value);

private:
	uint8_t command_;
};

#endif
