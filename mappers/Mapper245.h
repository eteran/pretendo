
#ifndef MAPPER245_20080314_H_
#define MAPPER245_20080314_H_

#include "Mapper.h"

class Mapper245 : public Mapper {
public:
	Mapper245();

public:
	virtual std::string name() const;

	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

	virtual void vram_change_hook(uint16_t vram_address);

private:
	int prg_bank(int bank) const;
	int chr_bank(int bank) const;

private:
	void clock_irq();

private:
	uint8_t chr_ram_[0x2000];
	uint8_t prg_ram_[0x2000];
	uint8_t chr_bank_[8];
	uint8_t prg_bank_[2];

	uint64_t prev_ppu_cycle_;
	uint16_t prev_vram_address_;

	uint8_t command_;
	uint8_t irq_latch_;
	uint8_t irq_counter_;
	bool irq_enabled_;
	bool irq_reload_;
	bool save_ram_enabled_;
	bool save_ram_writable_;
};

#endif
