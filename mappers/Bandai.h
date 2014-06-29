
#ifndef BANDAIR_20080314_H_
#define BANDAIR_20080314_H_

#include "Mapper.h"
#include "BitField.h"

class Bandai : public Mapper {
private:
	union IRQCounter {
		uint16_t raw;
		BitField<0,8> lo;
		BitField<8,8> hi;
	};
public:
	Bandai();

public:
	virtual std::string name() const;

public:
	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

public:
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
	void write_handler(uint16_t address, uint8_t value);
	uint8_t read_eeprom(uint16_t address);

private:
	uint8_t    chr_ram_[0x2000];
	IRQCounter irq_counter_;
	bool       irq_enabled_;

};

#endif
