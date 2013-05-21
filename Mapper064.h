
#ifndef MAPPER0_64_20080314_H_
#define MAPPER0_64_20080314_H_

#include "Mapper.h"


class Mapper64 : public Mapper {
public:
	Mapper64();

public:
	virtual std::string name() const;

	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);
	virtual void vram_change_hook(uint16_t vram_address);
	virtual void cpu_sync();

private:
	void sync_prg();
	void sync_chr();
	void clock_irq();

private:
	uint8_t chr_bank_[8];
	uint8_t prg_bank_[3];

	uint64_t prev_ppu_cycle_;
	uint16_t prev_vram_address_;

	uint8_t command_;
	uint8_t irq_latch_;
	uint8_t irq_counter_;
	uint8_t irq_mode_;
	uint64_t cpu_cycles_;
	bool irq_enabled_;
	bool irq_reload_;

};

#endif
