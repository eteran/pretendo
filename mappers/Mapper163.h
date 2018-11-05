
#ifndef MAPPER163_20080314_H_
#define MAPPER163_20080314_H_

#include "Mapper.h"

class Mapper163 final : public Mapper {
public:
	Mapper163();

public:
	std::string name() const override;

private:
	uint8_t read_5(uint16_t address) override;
	uint8_t read_6(uint16_t address) override;
	uint8_t read_7(uint16_t address) override;

public:
	void write_5(uint16_t address, uint8_t value) override;
	void write_6(uint16_t address, uint8_t value) override;
	void write_7(uint16_t address, uint8_t value) override;

private:
	uint8_t chr_ram_[0x2000];
	uint8_t prg_      = 0;
	uint8_t security_ = 0;
	uint8_t trigger_  = 0;
	uint8_t reg_5101_ = 0;

private:
    MemoryMappedFile prg_ptr_;
};

#endif
