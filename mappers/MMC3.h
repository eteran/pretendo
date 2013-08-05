
#ifndef MMC3_20120121_H_
#define MMC3_20120121_H_

#include "Mapper.h"

class MMC3 : public Mapper {
public:
	MMC3();

public:
	virtual std::string name() const;

public:
	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

public:
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);

public:
	virtual void vram_change_hook(uint16_t vram_address);

private:
	int prg_bank(int bank) const;
	int chr_bank(int bank) const;

private:
	virtual void clock_irq() = 0;

protected:
	uint8_t chr_ram_[0x10000]; // we should get this from iNES 2.0, 
							   // but this seems to do for now
	uint8_t prg_ram_[0x2000];
	uint8_t chr_bank_[8];
	uint8_t prg_bank_[2];

	uint64_t prev_ppu_cycle_;
	uint16_t prev_vram_address_;
	uint8_t  command_;
	uint8_t  irq_latch_;
	uint8_t  irq_counter_;
	bool     irq_enabled_;
	bool     irq_reload_;
	bool     save_ram_enabled_;
	bool     save_ram_writable_;
};


class MMC3A : public MMC3 {
private:
	virtual void clock_irq();
};

class MMC3B : public MMC3 {
private:
	virtual void clock_irq();
};

#endif
