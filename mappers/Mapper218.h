
#ifndef MAPPER218_20080314_H_
#define MAPPER218_20080314_H_

#include "Mapper.h"

class Mapper218 : public Mapper {
public:
	Mapper218();

public:
	virtual std::string name() const;

public:
	virtual uint8_t read_vram(uint16_t address);
	virtual void write_vram(uint16_t address, uint8_t value);
};

#endif
