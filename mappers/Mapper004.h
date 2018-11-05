
#ifndef MAPPER0004_20080314_H_
#define MAPPER0004_20080314_H_

#include "Mapper.h"
#include <memory>

class Mapper4 final : public Mapper {
public:
	Mapper4();

public:
	std::string name() const override;

public:
	uint8_t read_6(uint16_t address) override;
	uint8_t read_7(uint16_t address) override;

public:
	void write_6(uint16_t address, uint8_t value) override;
	void write_7(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;

public:
	void vram_change_hook(uint16_t vram_address) override;

private:
	std::unique_ptr<Mapper> mapper_;
};

#endif
