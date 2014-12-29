
#ifndef MAPPER090_20080314_H_
#define MAPPER090_20080314_H_

#include "Mapper.h"
#include "BitField.h"

class Mapper90 : public Mapper {
private:
	union BankControl {
		uint8_t raw;
		BitField<0,3> prg;
		BitField<3,2> chr;
		BitField<5>   n;
		BitField<6>   r;
		BitField<6>   map_67;
		
	};
	
	union ChrControl {
		uint8_t raw;
		BitField<0,5> block;	
		BitField<5>   disabled;
		BitField<7>   mirror;
	};
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
	uint8_t chr_hi_[8];
	uint8_t chr_lo_[8];
	uint8_t prg_[4];

	BankControl bank_control_;
	ChrControl  chr_control_;

	uint8_t multiply_1_;
	uint8_t multiply_2_;
	uint8_t prg_ram_;
};

#endif
