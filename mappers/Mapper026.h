
#ifndef MAPPER026_20080314_H_
#define MAPPER026_20080314_H_

#include "VRC6.h"

class Mapper26 final : public VRC6 {
public:
	Mapper26() = default;

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
};

#endif
