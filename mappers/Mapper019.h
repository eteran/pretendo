
#ifndef MAPPER019_20080314_H_
#define MAPPER019_20080314_H_

#include "Mapper.h"
#include "BitField.h"

class Mapper19 final : public Mapper {
private:
	union IRQControl {
		uint16_t raw;
		BitField<0,15> counter;
		BitField<15>   enabled;	
		
		// meta-fields
		BitField<0,8> lo;
		BitField<8,8> hi;			
	};
	
public:
	Mapper19();

public:
	std::string name() const override;

	uint8_t read_4(uint16_t address) override;
	uint8_t read_5(uint16_t address) override;
	uint8_t read_6(uint16_t address) override;
	uint8_t read_7(uint16_t address) override;

public:
	void write_4(uint16_t address, uint8_t value) override;
	void write_5(uint16_t address, uint8_t value) override;
	void write_6(uint16_t address, uint8_t value) override;
	void write_7(uint16_t address, uint8_t value) override;
	void write_8(uint16_t address, uint8_t value) override;
	void write_9(uint16_t address, uint8_t value) override;
	void write_a(uint16_t address, uint8_t value) override;
	void write_b(uint16_t address, uint8_t value) override;
	void write_c(uint16_t address, uint8_t value) override;
	void write_d(uint16_t address, uint8_t value) override;
	void write_e(uint16_t address, uint8_t value) override;
	void write_f(uint16_t address, uint8_t value) override;

public:
	void cpu_sync() override;

private:
	uint8_t    chr_ram_[0x2000];
	IRQControl irq_control_ = { 0 };
	uint8_t    mirroring_   = 0;
	MemoryMappedFile  prg_ptr_;
};

#endif
