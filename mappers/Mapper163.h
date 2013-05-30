
#ifndef MAPPER163_20080314_H_
#define MAPPER163_20080314_H_

#include "Mapper.h"

class Mapper163 : public Mapper {
public:
	Mapper163();

public:
	virtual std::string name() const;

private:
	virtual uint8_t read_5(uint16_t address);
	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

public:
	virtual void write_5(uint16_t address, uint8_t value);
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);

private:
	uint8_t chr_ram_[0x2000];
	uint8_t prg_ram_[0x2000];
	uint8_t prg_;
	uint8_t security_;
	uint8_t trigger_;
	uint8_t reg_5101_;
};

#endif
