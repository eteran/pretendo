
#ifndef MAPPER50_20080314_H_
#define MAPPER50_20080314_H_

#include "Mapper.h"

class Mapper50 : public Mapper {
public:
	Mapper50();

public:
	virtual std::string name() const;

	virtual void write_4(uint16_t address, uint8_t value);
	virtual void write_5(uint16_t address, uint8_t value);

public:
	virtual void cpu_sync();

private:
	void write_handler(uint16_t address, uint8_t value);

private:
	uint8_t  chr_ram_[0x2000];
	uint16_t irq_counter_;
	bool     irq_enabled_;
};

#endif
