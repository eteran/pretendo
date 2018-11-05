
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
	std::string name() const override;

public:
	uint8_t read_6(uint16_t address) override;
	uint8_t read_7(uint16_t address) override;

public:
	void write_6(uint16_t address, uint8_t value) override;
	void write_7(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;
	void write_f(uint16_t address, uint8_t value) override;

public:
	void cpu_sync() override;

private:
	void write_handler(uint16_t address, uint8_t value);
	uint8_t read_eeprom(uint16_t address);

private:
	uint8_t    chr_ram_[0x2000];
	IRQCounter irq_counter_ = { 0 };
	bool       irq_enabled_ = false;
};

#endif
