
#ifndef MAPPER090_20080314_H_
#define MAPPER090_20080314_H_

#include "BitField.h"
#include "Mapper.h"

class Mapper90 final : public Mapper {
private:
	union BankControl {
		uint8_t raw;
		BitField<0, 3> prg;
		BitField<3, 2> chr;
		BitField<5> n;
		BitField<6> r;
		BitField<6> map_67;
	};

	union ChrControl {
		uint8_t raw;
		BitField<0, 5> block;
		BitField<5> disabled;
		BitField<7> mirror;
	};

public:
	Mapper90();

public:
	std::string name() const override;

public:
	uint8_t read_5(uint16_t address) override;

public:
	void write_5(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;

public:
	uint8_t read_vram(uint16_t address) override;

private:
	void sync_prg();
	void sync_chr();

private:
	uint8_t chr_hi_[8] = {};
	uint8_t chr_lo_[8] = {};
	uint8_t prg_[4]    = {};

	BankControl bank_control_ = {0};
	ChrControl chr_control_   = {0};

	uint8_t multiply_1_ = 0;
	uint8_t multiply_2_ = 0;
	uint8_t prg_ram_    = 0;
};

#endif
