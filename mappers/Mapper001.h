
#ifndef MAPPER0001_20080314_H_
#define MAPPER0001_20080314_H_

#include "Mapper.h"

class Mapper1 : public Mapper {
public:
	Mapper1();

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

private:
	void write_handler(uint16_t address, uint8_t value);

private:
	uint8_t  chr_ram_[0x2000];
	uint8_t  regs_[4];
	uint64_t cpu_cycles_;
	uint8_t  latch_;
	uint8_t  write_counter_;
    MemoryMappedFile prg_ptr_;
	
};

#endif
