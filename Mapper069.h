
#ifndef MAPPER069_20080314_H_
#define MAPPER069_20080314_H_

#include "Mapper.h"

class Mapper69 : public Mapper {
public:
	Mapper69();

public:
	virtual std::string name() const;

	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);

public:
	virtual void cpu_sync();

private:
	uint8_t  chr_ram_[0x2000];
	uint8_t  prg_ram_[0x2000];
	uint16_t irq_counter_;
	uint8_t  irq_control_;
	uint8_t  command_8000_;
	uint8_t  command_c000_;
	uint8_t  prg_mode_;
};

#endif
