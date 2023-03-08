
#ifndef MAPPER090_20080314_H_
#define MAPPER090_20080314_H_

#include "BitField.h"
#include "Mapper.h"

class Mapper90 final : public Mapper {
private:
	union BankControl {
		uint8_t raw;
		BitField<uint8_t, 0, 3> prg;
		BitField<uint8_t, 3, 2> chr;
		BitField<uint8_t, 5> n;
		BitField<uint8_t, 6> r;
		BitField<uint8_t, 6> map_67;
	};

	union ChrControl {
		uint8_t raw;
		BitField<uint8_t, 0, 5> block;
		BitField<uint8_t, 5> disabled;
		BitField<uint8_t, 7> mirror;
	};

public:
	Mapper90();

public:
	std::string name() const override;

public:
	uint8_t read_5(uint32_t address) override;

public:
	void write_5(uint32_t address, uint8_t value) override;
	void write_8(uint32_t address, uint8_t value) override;
	void write_9(uint32_t address, uint8_t value) override;
	void write_a(uint32_t address, uint8_t value) override;
	void write_b(uint32_t address, uint8_t value) override;
	void write_c(uint32_t address, uint8_t value) override;
	void write_d(uint32_t address, uint8_t value) override;

public:
	uint8_t read_vram(uint32_t address) override;

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
