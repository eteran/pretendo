
#ifndef MAPPER011_20080314_H_
#define MAPPER011_20080314_H_

#include "Mapper.h"

class Mapper11 final : public Mapper {
public:
	Mapper11();

public:
	std::string name() const override;

	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;
	void write_f(uint16_t address, uint8_t value) override;

private:
	void write_handler(uint16_t address, uint8_t value);
};

#endif
