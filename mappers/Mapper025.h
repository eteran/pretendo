
#ifndef MAPPER025_20080314_H_
#define MAPPER025_20080314_H_

#include "Mapper.h"

class Mapper25 final : public Mapper {
public:
	Mapper25();

public:
	std::string name() const override;

public:
	uint8_t read_6(uint16_t address) override;

public:
	void write_6(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;

private:
	std::unique_ptr<Mapper> mapper_;
};

#endif
