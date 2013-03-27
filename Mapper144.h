
#ifndef MAPPER144_20080314_H_
#define MAPPER144_20080314_H_

#include "Mapper.h"

class Mapper144 : public Mapper {
public:
	Mapper144();

public:
	virtual std::string name() const;

	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

private:
	virtual void write_handler(uint16_t address, uint8_t value);
};

#endif
