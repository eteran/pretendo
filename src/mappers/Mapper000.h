
#ifndef MAPPER0000_20080314_H_
#define MAPPER0000_20080314_H_

#include "Mapper.h"

class Mapper0 final : public Mapper {
public:
	Mapper0();

public:
	std::string name() const override;

private:
	uint8_t chr_ram_[0x2000] = {};
};

#endif
