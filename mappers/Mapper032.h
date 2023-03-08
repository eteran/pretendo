
#ifndef MAPPER032_20080314_H_
#define MAPPER032_20080314_H_

#include "Mapper.h"

class Mapper32 final : public Mapper {
public:
	Mapper32();

public:
	std::string name() const override;

	void write_8(uint32_t address, uint8_t value) override;
	void write_9(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_b(uint32_t address, uint8_t value) override;

private:
	void sync_prg();

private:
	uint8_t prg_8000_ = 0;
	uint8_t prg_a000_ = 0;
	uint8_t prg_mode_ = 0;
};

#endif
