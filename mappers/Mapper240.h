
#ifndef MAPPER240_20080314_H_
#define MAPPER240_20080314_H_

#include "Mapper.h"

class Mapper240 final : public Mapper {
public:
	Mapper240();

public:
	std::string name() const override;

public:
	void write_4(uint32_t address, uint8_t value) override;
	void write_5(uint32_t address, uint8_t value) override;

private:
	void write_handler(uint32_t address, uint8_t value);
};

#endif
