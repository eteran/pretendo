
#ifndef MAPPER019_20080314_H_
#define MAPPER019_20080314_H_

#include "BitField.h"
#include "Mapper.h"

class Mapper19 final : public Mapper {
private:
	union IRQControl {
		uint16_t raw;
		BitField<uint16_t, 0, 15> counter;
		BitField<uint16_t, 15> enabled;

		// meta-fields
		BitField<uint16_t, 0, 8> lo;
		BitField<uint16_t, 8, 8> hi;
	};

public:
	Mapper19();

public:
	std::string name() const override;

	uint8_t read_4(uint_least16_t address) override;
	uint8_t read_5(uint_least16_t address) override;
	uint8_t read_6(uint_least16_t address) override;
	uint8_t read_7(uint_least16_t address) override;

public:
	void write_4(uint_least16_t address, uint8_t value) override;
	void write_5(uint_least16_t address, uint8_t value) override;
	void write_6(uint_least16_t address, uint8_t value) override;
	void write_7(uint_least16_t address, uint8_t value) override;
	void write_8(uint_least16_t address, uint8_t value) override;
	void write_9(uint_least16_t address, uint8_t value) override;
	void write_a(uint_least16_t address, uint8_t value) override;
	void write_b(uint_least16_t address, uint8_t value) override;
	void write_c(uint_least16_t address, uint8_t value) override;
	void write_d(uint_least16_t address, uint8_t value) override;
	void write_e(uint_least16_t address, uint8_t value) override;
	void write_f(uint_least16_t address, uint8_t value) override;

public:
	void cpu_sync() override;

private:
	uint8_t chr_ram_[0x2000] = {};
	IRQControl irq_control_  = {0};
	uint8_t mirroring_       = 0;
	MemoryMappedFile prg_ptr_;
};

#endif
