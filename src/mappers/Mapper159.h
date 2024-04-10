
#ifndef MAPPER159_20080314_H_
#define MAPPER159_20080314_H_

#include "Bandai.h"

class Mapper159 : public Bandai {
public:
	Mapper159() = default;

public:
	std::string name() const override;
};

#endif
