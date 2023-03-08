
#ifndef MAPPER034_20080314_H_
#define MAPPER034_20080314_H_

#include "Mapper.h"

class Mapper34 final : public Mapper {
public:
	Mapper34();

public:
	std::string name() const override;

	void write_7(uint32_t address, uint8_t value) override;
	void write_8(uint32_t address, uint8_t value) override;
	void write_9(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_b(uint32_t address, uint8_t value) override;
	void write_c(uint32_t address, uint8_t value) override;
	void write_d(uint32_t address, uint8_t value) override;
	void write_e(uint32_t address, uint8_t value) override;
	void write_f(uint32_t address, uint8_t value) override;

private:
	void write_handler(uint32_t address, uint8_t value);

private:
	uint8_t chr_ram_[0x2000] = {};

private:
	enum Mode {
		NINA_001,
		BNROM,
	} mode_;
};

#endif
