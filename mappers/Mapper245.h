
#ifndef MAPPER245_20080314_H_
#define MAPPER245_20080314_H_

#include "Mapper.h"

class Mapper245 final : public Mapper {
public:
	Mapper245();

public:
	std::string name() const override;

	uint8_t read_6(uint16_t address) override;
	uint8_t read_7(uint16_t address) override;

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

	void vram_change_hook(uint16_t vram_address) override;

private:
	int prg_bank(int bank) const;
	int chr_bank(int bank) const;

private:
	void clock_irq();

private:
	uint8_t chr_ram_[0x2000] = {};
	uint8_t chr_bank_[8] = {};
	uint8_t prg_bank_[2] = {};

	uint64_t prev_ppu_cycle_    = 0;
	uint16_t prev_vram_address_ = 0xffff;

	uint8_t command_            = 0;
	uint8_t irq_latch_          = 0;
	uint8_t irq_counter_        = 0;
	bool irq_enabled_           = false;
	bool irq_reload_            = false;
	bool save_ram_enabled_      = true;
	bool save_ram_writable_     = true;

private:
    MemoryMappedFile prg_ptr_;
};

#endif
