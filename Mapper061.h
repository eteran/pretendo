
#ifndef MAPPER061_20080314_H_
#define MAPPER061_20080314_H_

#include "Mapper.h"

class Mapper61 : public Mapper {
public:
	Mapper61();

public:
	virtual std::string name() const;

public:
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

private:
	void write_hander(uint16_t address, uint8_t value);

private:
	uint8_t chr_ram_[0x2000];
};

#endif
