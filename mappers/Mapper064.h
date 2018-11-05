
#ifndef MAPPER0_64_20080314_H_
#define MAPPER0_64_20080314_H_

#include "Mapper.h"

class Mapper64 final : public Mapper {
public:
	Mapper64();

public:
	std::string name() const override;

	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;
	void write_f(uint16_t address, uint8_t value) override;
	void vram_change_hook(uint16_t vram_address) override;
	void cpu_sync() override;

private:
	void sync_prg();
	void sync_chr();
	void clock_irq();

private:
	uint8_t chr_bank_[8];
	uint8_t prg_bank_[3];

	uint64_t cpu_cycles_        = 0;
	uint64_t prev_ppu_cycle_    = 0;
	uint16_t prev_vram_address_ = 0xffff;
	uint8_t command_            = 0;
	uint8_t irq_latch_          = 0;
	uint8_t irq_counter_        = 0;
	uint8_t irq_mode_           = 0;
	bool irq_enabled_           = false;
	bool irq_reload_            = false;
};

#endif
