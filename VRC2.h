
#ifndef VRC2_20120121_H_
#define VRC2_20120121_H_

#include "Mapper.h"

class VRC2 : public Mapper {
public:
	VRC2(int chr_shift);

public:
	virtual std::string name() const;

public:
	virtual uint8_t read_6(uint16_t address);

public:
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);

private:
	uint8_t chr_[8];
	uint8_t latch_;
	int     chr_shift_;
};

#endif
