
#ifndef MAPPER0005_20080314_H_
#define MAPPER0005_20080314_H_

#include "Mapper.h"

class Mapper5 : public Mapper {
public:
	Mapper5();

public:
	virtual std::string name() const;

public:
	virtual void write_2(uint16_t address, uint8_t value);
	virtual void write_3(uint16_t address, uint8_t value);
	virtual void write_5(uint16_t address, uint8_t value);
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);

public:
	virtual uint8_t read_5(uint16_t address);
	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);
	virtual uint8_t read_8(uint16_t address);
	virtual uint8_t read_9(uint16_t address);
	virtual uint8_t read_a(uint16_t address);
	virtual uint8_t read_b(uint16_t address);
	virtual uint8_t read_c(uint16_t address);
	virtual uint8_t read_d(uint16_t address);

public:
	virtual uint8_t read_vram(uint16_t address);
	virtual void write_vram(uint16_t address, uint8_t value);

public:
	virtual void vram_change_hook(uint16_t vram_address);
	virtual void ppu_end_frame();

private:
	void clock_irq();
	void write_handler(uint16_t address, uint8_t value);
	uint8_t read_handler(uint16_t address);

private:

	uint8_t  prg_ram_[2][0x8000];
	uint8_t  exram_[0x400];
	uint8_t  bg_chr_banks_[8];
	uint8_t  sp_chr_banks_[8];
	uint16_t prev_vram_address_[2];
	uint8_t *prg_ram_banks_[0x10];

	bool     irq_enabled_;
	uint16_t bg_char_upper_;
	uint16_t fetch_count_;
	uint8_t  chr_mode_;
	uint8_t  exram_mode_;
	uint8_t  fill_mode_attr_;
	uint8_t  fill_mode_tile_;
	uint8_t  irq_counter_;
	uint8_t  irq_status_;
	uint8_t  irq_target_;
	uint8_t  mirroring_mode_;
	uint8_t  multiplier_1_;
	uint8_t  multiplier_2_;
	uint8_t  prg_mode_;
	uint8_t  prg_ram_protect1_;
	uint8_t  prg_ram_protect2_;
	uint8_t  sprite_size_;
	uint8_t  vertical_split_mode_;
	uint8_t  vertical_split_scroll_;
	uint8_t  vertical_split_bank_;

	enum {
		CHR_BANK_A,
		CHR_BANK_B
	} last_chr_write_;
};

#endif
