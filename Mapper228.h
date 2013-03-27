
#ifndef MAPPER228_20080314_H_
#define MAPPER228_20080314_H_

#include "Mapper.h"

class Mapper228 : public Mapper {
public:
	Mapper228();

public:
	virtual std::string name() const;

public:
	virtual void write_4(uint16_t address, uint8_t value);
	virtual void write_5(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

private:
	virtual uint8_t read_4(uint16_t address);
	virtual uint8_t read_5(uint16_t address);

private:
	void write_hander(uint16_t address, uint8_t value);

private:
	uint8_t ram_[0x04];
};

#endif
