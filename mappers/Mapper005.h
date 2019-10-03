
#ifndef MAPPER005_20080314_H_
#define MAPPER005_20080314_H_

#include "Mapper.h"
#include "BitField.h"

class Mapper5 final : public Mapper {
private:
	union IRQStatus {
		uint8_t raw;
		BitField<6> in_frame;
		BitField<7> pending;
	};
public:
	Mapper5();

public:
	std::string name() const override;

public:
	void write_2(uint16_t address, uint8_t value) override;
	void write_3(uint16_t address, uint8_t value) override;
	void write_5(uint16_t address, uint8_t value) override;
	void write_6(uint16_t address, uint8_t value) override;
	void write_7(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;

public:
	uint8_t read_5(uint16_t address) override;
	uint8_t read_6(uint16_t address) override;
	uint8_t read_7(uint16_t address) override;
	uint8_t read_8(uint16_t address) override;
	uint8_t read_9(uint16_t address) override;
	uint8_t read_a(uint16_t address) override;
	uint8_t read_b(uint16_t address) override;
	uint8_t read_c(uint16_t address) override;
	uint8_t read_d(uint16_t address) override;

public:
	uint8_t read_vram(uint16_t address) override;
	void write_vram(uint16_t address, uint8_t value) override;

public:
	void vram_change_hook(uint16_t vram_address) override;
	void ppu_end_frame() override;

private:
	void clock_irq();
	void write_handler(uint16_t address, uint8_t value);
	uint8_t read_handler(uint16_t address);

private:

	uint8_t   prg_ram_[2][0x8000]   = {};
	uint8_t   exram_[0x400]         = {};
	uint8_t   bg_chr_banks_[8]      = {};
	uint8_t   sp_chr_banks_[8]      = {};
	uint16_t  prev_vram_address_[2] = { 0xffff, 0xffff };
	uint8_t * prg_ram_banks_[0x10]  = {};

	bool      irq_enabled_           = false;
	bool      large_sprites_         = false;
	uint16_t  bg_char_upper_         = 0x00;
	uint16_t  fetch_count_           = 0;
	uint8_t   chr_mode_              = 0;
	uint8_t   exram_mode_            = 0;
	uint8_t   fill_mode_attr_        = 0;
	uint8_t   fill_mode_tile_        = 0;
	uint8_t   irq_counter_           = 0;
	IRQStatus irq_status_            = {0};
	uint8_t   irq_target_            = 0;
	uint8_t   mirroring_mode_        = 0;
	uint8_t   multiplier_1_          = 0;
	uint8_t   multiplier_2_          = 0;
	uint8_t   prg_mode_              = 0x03;
	uint8_t   prg_ram_protect1_      = 0;
	uint8_t   prg_ram_protect2_      = 0;
	uint8_t   vertical_split_mode_   = 0;
	uint8_t   vertical_split_scroll_ = 0;
	uint8_t   vertical_split_bank_   = 0;

	enum {
		CHR_BANK_A,
		CHR_BANK_B
	} last_chr_write_ = CHR_BANK_A;
};

#endif
