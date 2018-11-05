
#ifndef VRC2_20120121_H_
#define VRC2_20120121_H_

#include "Mapper.h"

class VRC2 : public Mapper {
public:
	explicit VRC2(int chr_shift);

public:
	std::string name() const override;

public:
	uint8_t read_6(uint16_t address) override;

public:
	void write_6(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;

private:
	uint8_t chr_[8];
	uint8_t latch_ = 0;
	int     chr_shift_;
};

#endif
