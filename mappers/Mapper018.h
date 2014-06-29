
#ifndef MAPPER018_20080314_H_
#define MAPPER018_20080314_H_

#include "Mapper.h"
#include "BitField.h"

class Mapper18 : public Mapper {
private:
	union IRQControl {
		uint8_t raw;
		BitField<0> enabled;
		BitField<1> mode_12bit;
		BitField<2> mode_8bit;
		BitField<3> mode_4bit;
	};
	
	union IRQCounter {
		uint16_t raw;
		BitField<0,4>  counter_4bit;
		BitField<0,8>  counter_8bit;
		BitField<0,12> counter_12bit;
		BitField<0,16> counter_16bit;
	};
	
	union IRQLatch {
		uint16_t raw;
		BitField<0,4>   byte1;
		BitField<4,8>   byte2;
		BitField<8,12>  byte3;
		BitField<12,16> byte4;
	};
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
	uint8_t    chr_ram_[0x2000];
	uint8_t    chr_[16];
	uint8_t    prg_[8]; // upper 2 are unneccessary, but simpler this way
	IRQCounter irq_counter_;
	IRQLatch   irq_latch_;
	IRQControl irq_control_;
};

#endif
