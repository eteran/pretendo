
#ifndef MAPPER025_20080314_H_
#define MAPPER025_20080314_H_

#include "VRC2.h"

class Mapper25 final : public VRC2 {
public:
	Mapper25();

public:
	std::string name() const override;

public:
	uint8_t read_6(uint32_t address) override;

public:
	void write_6(uint32_t address, uint8_t value) override;
	void write_8(uint32_t address, uint8_t value) override;
	void write_9(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_b(uint32_t address, uint8_t value) override;
	void write_c(uint32_t address, uint8_t value) override;
	void write_d(uint32_t address, uint8_t value) override;
	void write_e(uint32_t address, uint8_t value) override;
};

#endif
