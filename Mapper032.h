
#ifndef MAPPER32_20080314_H_
#define MAPPER32_20080314_H_

#include "Mapper.h"

class Mapper32 : public Mapper {
public:
	Mapper32();

public:
	virtual std::string name() const;

	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);

private:
	void sync_prg();

private:
	uint8_t prg_8000_;
	uint8_t prg_a000_;
	uint8_t prg_mode_;
};

#endif
