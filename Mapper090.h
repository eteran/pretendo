
#ifndef MAPPER90_20080314_H_
#define MAPPER90_20080314_H_

#include "Mapper.h"

class Mapper90 : public Mapper {
public:
	Mapper90();

public:
	virtual std::string name() const;

public:
	virtual uint8_t read_5(uint16_t address);

public:
	virtual void write_5(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);

public:
	virtual uint8_t read_vram(uint16_t address);

private:
	void sync_prg();
	void sync_chr();

private:
	uint16_t chr_[8];
	uint8_t  prg_[4];

	uint8_t  map_67_;
	uint8_t  prg_mode_;
	uint8_t  chr_mode_;

	uint8_t  chr_mirror_;
	uint8_t  chr_block_mode_;
	uint8_t  chr_block_;
	
	uint8_t  multiply_1_;
	uint8_t  multiply_2_;
	uint8_t  prg_ram_;
};

#endif
