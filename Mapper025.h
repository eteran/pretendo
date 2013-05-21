
#ifndef MAPPER025_20080314_H_
#define MAPPER025_20080314_H_

#include "Mapper.h"

class Mapper25 : public Mapper {
public:
	Mapper25();
	virtual ~Mapper25();

public:
	std::string name() const;

public:
	virtual uint8_t read_6(uint16_t address);

public:
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);

private:
	Mapper *mapper_;
};

#endif
