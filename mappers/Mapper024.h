
#ifndef MAPPER024_20080314_H_
#define MAPPER024_20080314_H_

#include "BitField.h"
#include "VRC6.h"

class Mapper24 final : public VRC6 {
public:
	Mapper24() = default;

public:
	std::string name() const override;
};

#endif
