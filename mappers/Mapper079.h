
#ifndef MAPPER079_20080314_H_
#define MAPPER079_20080314_H_

#include "Mapper.h"

class Mapper79 final : public Mapper {
public:
	Mapper79();

public:
	std::string name() const override;

public:
	void write_4(uint_least16_t address, uint8_t value) override;
	void write_5(uint_least16_t address, uint8_t value) override;

private:
	void write_handler(uint_least16_t address, uint8_t value);
};

#endif
