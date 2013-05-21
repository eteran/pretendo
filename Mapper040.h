
#ifndef MAPPER040_20080314_H_
#define MAPPER040_20080314_H_

#include "Mapper.h"

class Mapper40 : public Mapper {
public:
	Mapper40();

public:
	virtual std::string name() const;

	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

public:
	virtual void cpu_sync();

private:
	uint16_t irq_counter_;
	bool irq_enabled_;
};

#endif
