
#ifndef MAPPER018_20080314_H_
#define MAPPER018_20080314_H_

#include "BitField.h"
#include "Mapper.h"

class Mapper18 final : public Mapper {
private:
	union IRQControl {
		uint8_t raw;
		BitField<uint8_t, 0> enabled;
		BitField<uint8_t, 1> mode_12bit;
		BitField<uint8_t, 2> mode_8bit;
		BitField<uint8_t, 3> mode_4bit;
	};

	union IRQCounter {
		uint16_t raw;
		BitField<uint16_t, 0, 4> counter_4bit;
		BitField<uint16_t, 0, 8> counter_8bit;
		BitField<uint16_t, 0, 12> counter_12bit;
		BitField<uint16_t, 0, 16> counter_16bit;
	};

	union IRQLatch {
		uint16_t raw;
		BitField<uint16_t, 0, 4> byte1;
		BitField<uint16_t, 4, 8> byte2;
		BitField<uint16_t, 8, 12> byte3;
		BitField<uint16_t, 12, 16> byte4;
	};

public:
	Mapper18();

public:
	std::string name() const override;

public:
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
	uint8_t chr_ram_[0x2000] = {};
	uint8_t chr_[16]         = {};
	uint8_t prg_[8]          = {}; // upper 2 are unneccessary, but simpler this way
	IRQCounter irq_counter_  = {0};
	IRQLatch irq_latch_      = {0};
	IRQControl irq_control_  = {0};
};

#endif
