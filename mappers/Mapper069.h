
#ifndef MAPPER069_20080314_H_
#define MAPPER069_20080314_H_

#include "BitField.h"
#include "Mapper.h"

class Mapper69 final : public Mapper {
public:
	union IRQControl {
		uint8_t raw;
		BitField<uint8_t, 0> enabled;
		BitField<uint8_t, 7> counter_enabled;
	};

	union IRQCounter {
		uint16_t raw;
		BitField<uint16_t, 0, 8> lo;
		BitField<uint16_t, 8, 8> hi;
	};

public:
	Mapper69();

public:
	std::string name() const override;

	uint8_t read_6(uint32_t address) override;
	uint8_t read_7(uint32_t address) override;

	void write_6(uint32_t address, uint8_t value) override;
	void write_7(uint32_t address, uint8_t value) override;
	void write_8(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_c(uint32_t address, uint8_t value) override;
	void write_e(uint32_t address, uint8_t value) override;

public:
	void cpu_sync() override;

private:
	uint8_t chr_ram_[0x2000] = {};
	IRQCounter irq_counter_  = {0};
	IRQControl irq_control_  = {0};
	uint8_t command_8000_    = 0;
	uint8_t command_c000_    = 0;
	uint8_t prg_mode_        = 0;

private:
	MemoryMappedFile prg_ptr_;
};

#endif
