
#ifndef MAPPER033_20080314_H_
#define MAPPER033_20080314_H_

#include "Mapper.h"

class Mapper33 final : public Mapper {
public:
	Mapper33();

public:
	std::string name() const override;

	void write_8(uint32_t address, uint8_t value) override;
	void write_9(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_b(uint32_t address, uint8_t value) override;

private:
	void write_handler(uint32_t address, uint8_t value);
};

#endif
