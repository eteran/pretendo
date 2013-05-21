
#ifndef MAPPER0004_20080314_H_
#define MAPPER0004_20080314_H_

#include "Mapper.h"

class Mapper4 : public Mapper {
public:
	Mapper4();
	virtual ~Mapper4();

public:
	virtual std::string name() const;

public:
	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

public:
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);

public:
	virtual void vram_change_hook(uint16_t vram_address);

private:
	Mapper *const mapper_;

};

#endif
