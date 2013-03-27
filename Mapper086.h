
#ifndef MAPPER86_20080314_H_
#define MAPPER86_20080314_H_

#include "Mapper.h"

class Mapper86 : public Mapper {
public:
	Mapper86();

public:
	virtual std::string name() const;

public:
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
};

#endif
