
#ifndef MAPPER018_20080314_H_
#define MAPPER018_20080314_H_

#include "Mapper.h"

class Mapper18 : public Mapper {
public:
	Mapper18();

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

public:
	virtual void cpu_sync();

private:
	uint8_t  chr_ram_[0x2000];
	uint8_t  chr_[16];
	uint8_t  prg_[8]; // upper 2 are unneccessary, but simpler this way
	uint16_t irq_counter_;
	uint16_t irq_latch_;
	uint16_t irq_mask_;
	uint8_t  irq_control_;
};

#endif
