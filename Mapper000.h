
#ifndef MAPPER0000_20080314_H_
#define MAPPER0000_20080314_H_

#include "Mapper.h"

class Mapper0 : public Mapper {
public:
	Mapper0();

public:
	virtual std::string name() const;

private:
	uint8_t chr_ram_[0x2000];
};

#endif
