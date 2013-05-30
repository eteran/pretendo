
#ifndef MAPPER033_20080314_H_
#define MAPPER033_20080314_H_

#include "Mapper.h"

class Mapper33 : public Mapper {
public:
	Mapper33();

public:
	virtual std::string name() const;

	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);

private:
	void write_handler(uint16_t address, uint8_t value);
};

#endif

