
#ifndef MAPPER019_20080314_H_
#define MAPPER019_20080314_H_

#include "Mapper.h"
#include "BitField.h"

class Mapper19 : public Mapper {
private:
	union IRQControl {
		uint16_t raw;
		BitField<0,15> counter;
		BitField<15>   enabled;	
		
		// meta-fields
		BitField<0,8> lo;
		BitField<8,8> hi;			
	};
	
public:
	Mapper19();

public:
	virtual std::string name() const;

	virtual uint8_t read_4(uint16_t address);
	virtual uint8_t read_5(uint16_t address);
	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

public:
	virtual void write_4(uint16_t address, uint8_t value);
	virtual void write_5(uint16_t address, uint8_t value);
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

public:
	virtual void cpu_sync();

private:
	uint8_t    prg_ram_[0x2000];
	uint8_t    chr_ram_[0x2000];
	IRQControl irq_control_;
	uint8_t    mirroring_;
};

#endif
