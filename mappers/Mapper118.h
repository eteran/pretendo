
#ifndef MAPPER118_20080314_H_
#define MAPPER118_20080314_H_

#include "Mapper.h"
#include <memory>

class Mapper118 final : public Mapper {
public:
	Mapper118();

public:
	std::string name() const override;

	uint8_t read_6(uint16_t address) override;
	uint8_t read_7(uint16_t address) override;

	void write_6(uint16_t address, uint8_t value) override;
	void write_7(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;
	void write_f(uint16_t address, uint8_t value) override;

	void vram_change_hook(uint16_t vram_address) override;

private:
	std::unique_ptr<Mapper> mapper_;

};

#endif
