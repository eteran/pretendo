
#ifndef MAPPER206_20080314_H_
#define MAPPER206_20080314_H_

#include "Mapper.h"

class Mapper206 final : public Mapper {
public:
	Mapper206();

public:
	std::string name() const override;

public:
	void write_8(uint_least16_t address, uint8_t value) override;

private:
	uint8_t command_ = 0;
};

#endif
