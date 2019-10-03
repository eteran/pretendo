
#ifndef MAPPER245_20080314_H_
#define MAPPER245_20080314_H_

#include "MMC3.h"

class Mapper245 final : public MMC3 {
public:
	Mapper245() = default;

public:
	std::string name() const override;
};

#endif
