
#ifndef BANDAIR_20080314_H_
#define BANDAIR_20080314_H_

#include "BitField.h"
#include "Mapper.h"

class Bandai : public Mapper {
private:
	union IRQCounter {
		uint16_t raw;
		BitField<uint16_t, 0, 8> lo;
		BitField<uint16_t, 8, 8> hi;
	};

public:
	Bandai();

public:
	std::string name() const override;

public:
	uint8_t read_6(uint32_t address) override;
	uint8_t read_7(uint32_t address) override;

public:
	void write_6(uint32_t address, uint8_t value) override;
	void write_7(uint32_t address, uint8_t value) override;
	void write_8(uint32_t address, uint8_t value) override;
	void write_9(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_b(uint32_t address, uint8_t value) override;
	void write_c(uint32_t address, uint8_t value) override;
	void write_d(uint32_t address, uint8_t value) override;
	void write_e(uint32_t address, uint8_t value) override;
	void write_f(uint32_t address, uint8_t value) override;

public:
	void cpu_sync() override;

private:
	void write_handler(uint32_t address, uint8_t value);
	uint8_t read_eeprom(uint32_t address);

private:
	uint8_t chr_ram_[0x2000] = {};
	IRQCounter irq_counter_  = {0};
	bool irq_enabled_        = false;
};

#endif
