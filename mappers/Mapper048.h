
#ifndef MAPPER048_20080314_H_
#define MAPPER048_20080314_H_

#include "Mapper.h"

class Mapper48 final : public Mapper {
public:
	Mapper48();

public:
	std::string name() const override;

	void write_8(uint32_t address, uint8_t value) override;
	void write_9(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_b(uint32_t address, uint8_t value) override;
	void write_c(uint32_t address, uint8_t value) override;
	void write_d(uint32_t address, uint8_t value) override;
	void write_e(uint32_t address, uint8_t value) override;
	void write_f(uint32_t address, uint8_t value) override;

	void vram_change_hook(uint32_t vram_address) override;

private:
	void write_handler(uint32_t address, uint8_t value);

private:
	void clock_irq();

private:
	uint64_t prev_ppu_cycle_    = 0;
	uint32_t prev_vram_address_ = 0xffff;
	uint8_t irq_latch_          = 0;
	uint8_t irq_counter_        = 0;
	bool irq_enabled_           = false;
	bool irq_reload_            = false;
};

#endif
