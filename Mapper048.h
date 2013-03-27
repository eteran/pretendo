
#ifndef MAPPER48_20080314_H_
#define MAPPER48_20080314_H_

#include "Mapper.h"

class Mapper48 : public Mapper {
public:
	Mapper48();

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

private:
	void write_handler(uint16_t address, uint8_t value);

private:
	void clock_irq();

private:
	uint64_t prev_ppu_cycle_;
	uint16_t prev_vram_address_;
	uint8_t irq_latch_;
	uint8_t irq_counter_;
	bool irq_enabled_;
	bool irq_reload_;
};

#endif

