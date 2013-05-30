
#ifndef MAPPER118_20080314_H_
#define MAPPER118_20080314_H_

#include "Mapper.h"

class Mapper118 : public Mapper {
public:
	Mapper118();
	virtual ~Mapper118();

public:
	virtual std::string name() const;

	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

	virtual void vram_change_hook(uint16_t vram_address);

private:
	Mapper *const mapper_;

};

#endif
