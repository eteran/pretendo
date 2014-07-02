
#ifndef CPU_20080314_H_
#define CPU_20080314_H_

#include "bender.h"

#include <cstdint>

class Mapper;

class CPU {
public:
	static const unsigned int page_count = 8;
	static const unsigned int page_size  = 0x10000 / page_count;
	static const unsigned int page_mask  = page_size - 1;
	static const unsigned int page_shift = 13;
	
private:
	enum {
		C_MASK = 0x01,
		Z_MASK = 0x02,
		I_MASK = 0x04,
		D_MASK = 0x08,
		B_MASK = 0x10,
		R_MASK = 0x20,	// antisocial flag... always 1
		V_MASK = 0x40,
		N_MASK = 0x80
	};

public:
	enum IRQ_SOURCE {
		MAPPER_IRQ = 0x01,
		APU_IRQ    = 0x02,
		FDS_IRQ    = 0x04,
		ALL_IRQ    = 0xff
	};

public:
	CPU();
	~CPU();

public:
	void write(uint16_t address, uint8_t value);
	uint8_t read(uint16_t address);

public:
	uint16_t pc() const { return rPC;}
	uint8_t a() const   { return rA; }
	uint8_t p() const   { return rP; }
	uint8_t s() const   { return rS; }
	uint8_t x() const   { return rX; }
	uint8_t y() const   { return rY; }

	// uneccessary, but can help keep debugger code clean
	bool flag_c() const { return (p() & C_MASK) != 0; }
	bool flag_d() const { return (p() & D_MASK) != 0; }
	bool flag_i() const { return (p() & I_MASK) != 0; }
	bool flag_n() const { return (p() & N_MASK) != 0; }
	bool flag_r() const { return true; }
	bool flag_v() const { return (p() & V_MASK) != 0; }
	bool flag_z() const { return (p() & Z_MASK) != 0; }

public:
	void nmi()                   { bender_nmi(); }
	void irq(IRQ_SOURCE source);
	void clear_irq(IRQ_SOURCE source);
	void burn(int cycles)        { bender_eatcycles(cycles); }
	uint64_t cycle_count() const { return bender_elapsedcycles(0); }
	int exec(int cycles);
	void stop();

public:
	// 8 k pages
	void swap_01(uint8_t *ptr) const	{ bender_memory[0x00] = ptr; }
	void swap_23(uint8_t *ptr) const	{ bender_memory[0x01] = ptr; }
	void swap_45(uint8_t *ptr) const	{ bender_memory[0x02] = ptr; }
	void swap_67(uint8_t *ptr) const	{ bender_memory[0x03] = ptr; }
	void swap_89(uint8_t *ptr) const	{ bender_memory[0x04] = ptr; }
	void swap_ab(uint8_t *ptr) const	{ bender_memory[0x05] = ptr; }
	void swap_cd(uint8_t *ptr) const	{ bender_memory[0x06] = ptr; }
	void swap_ef(uint8_t *ptr) const	{ bender_memory[0x07] = ptr; }

	void unmap_01() const	{ bender_memory[0x00] = 0; }
	void unmap_23() const	{ bender_memory[0x01] = 0; }
	void unmap_45() const	{ bender_memory[0x02] = 0; }
	void unmap_67() const	{ bender_memory[0x03] = 0; }
	void unmap_89() const	{ bender_memory[0x04] = 0; }
	void unmap_ab() const	{ bender_memory[0x05] = 0; }
	void unmap_cd() const	{ bender_memory[0x06] = 0; }
	void unmap_ef() const	{ bender_memory[0x07] = 0; }
	
public:
	void trash_ram();
	void reset();
	
private:
	uint8_t ram_[0x800];
	int cycle_;
	uint8_t irq_sources_;
};

#endif
