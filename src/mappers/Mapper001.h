
#ifndef MAPPER0001_20080314_H_
#define MAPPER0001_20080314_H_

#include "Mapper.h"

class Mapper1 final : public Mapper {
public:
	Mapper1();

public:
	std::string name() const override;

	uint8_t read_6(uint_least16_t address) override;
	uint8_t read_7(uint_least16_t address) override;

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

private:
	void write_handler(uint_least16_t address, uint8_t value);

private:
	uint8_t chr_ram_[0x2000] = {};
	uint8_t regs_[4]         = {};
	uint64_t cpu_cycles_     = 0;
	uint8_t latch_           = 0;
	uint8_t write_counter_   = 0;
	uint8_t prg_ram_enable0_ = 0x10;
	uint8_t prg_ram_enable1_ = 0x10;

private:
	MemoryMappedFile prg_ptr_;
};

#endif
