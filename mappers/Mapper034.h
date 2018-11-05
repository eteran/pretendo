
#ifndef MAPPER034_20080314_H_
#define MAPPER034_20080314_H_

#include "Mapper.h"
#include <memory>

class Mapper34 final : public Mapper {
public:
	Mapper34();

public:
	std::string name() const override;

	void write_7(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;
	void write_f(uint16_t address, uint8_t value) override;

private:
	std::unique_ptr<Mapper> mapper_;
};

#endif

