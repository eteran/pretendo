
#ifndef MAPPER68_20080314_H_
#define MAPPER68_20080314_H_

#include "Mapper.h"

class Mapper68 : public Mapper {
public:
	Mapper68();

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

	virtual uint8_t read_vram(uint16_t address);

private:
	uint8_t chr_rom_reg_[2];
	uint8_t reg_e000_;
};

#endif
