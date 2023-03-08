
#ifndef MAPPER013_20080314_H_
#define MAPPER013_20080314_H_

#include "Mapper.h"

class Mapper13 final : public Mapper {
public:
	Mapper13();

public:
	std::string name() const override;

public:
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
	uint8_t chr_ram_[4 * 0x1000] = {}; // 4 4KB pages
};

#endif
