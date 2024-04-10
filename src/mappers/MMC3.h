
#ifndef MMC3_20080314_H_
#define MMC3_20080314_H_

#include "Mapper.h"

class MMC3 : public Mapper {
public:
	MMC3();

public:
	std::string name() const override;

public:
	uint8_t read_6(uint_least16_t address) override;
	uint8_t read_7(uint_least16_t address) override;

public:
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
	void vram_change_hook(uint_least16_t vram_address) override;

private:
	int prg_bank(int bank) const;
	int chr_bank(int bank) const;

private:
	void clock_irqA();
	void clock_irqB();
	void clock_irq();

protected:
	uint8_t chr_ram_[0x40000] = {}; // we should get this from iNES 2.0,
									// but this seems to do for now
	uint8_t chr_bank_[8] = {};
	uint8_t prg_bank_[2] = {};

	uint64_t prev_ppu_cycle_    = 0;
	uint_least16_t prev_vram_address_ = 0xffff;
	uint8_t command_            = 0;
	uint8_t irq_latch_          = 0;
	uint8_t irq_counter_        = 0;
	bool irq_enabled_           = false;
	bool irq_reload_            = false;
	bool save_ram_enabled_      = false;
	bool save_ram_writable_     = false;

private:
	MemoryMappedFile prg_ptr_;

protected:
	enum Mode {
		ModeA,
		ModeB,
		ModeMMC6,
	} mode_ = ModeA;
};

#endif
