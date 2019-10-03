
#ifndef MAPPER004_20080314_H_
#define MAPPER004_20080314_H_

#include "MMC3.h"

class Mapper4 final : public MMC3 {
public:
	Mapper4();

public:
	std::string name() const override;
};

#endif
