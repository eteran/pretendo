
#ifndef MAPPER016_20080314_H_
#define MAPPER016_20080314_H_

#include "Bandai.h"

class Mapper16 final : public Bandai {
public:
	Mapper16() = default;

public:
	std::string name() const override;
};

#endif
