
#ifndef CPU_20080314_H_
#define CPU_20080314_H_

#include "../Reset.h"
#include "P6502.h"

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Mapper;

class CPU {
public:
	static const unsigned int page_count = 32;
	static const unsigned int page_size  = 0x10000 / page_count;
	static const unsigned int page_mask  = page_size - 1;
	static const unsigned int page_shift = 11;

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
	uint8_t read(uint16_t address) const;

public:
	uint16_t pc() const { return P6502::PC;}
	uint8_t a() const   { return P6502::A; }
	uint8_t p() const   { return P6502::P; }
	uint8_t s() const   { return P6502::S; }
	uint8_t x() const   { return P6502::X; }
	uint8_t y() const   { return P6502::Y; }

	// uneccessary, but can help keep debugger code clean
	bool flag_c() const { return (p() & C_MASK) != 0; }
	bool flag_d() const { return (p() & D_MASK) != 0; }
	bool flag_i() const { return (p() & I_MASK) != 0; }
	bool flag_n() const { return (p() & N_MASK) != 0; }
	bool flag_r() const { return true; }
	bool flag_v() const { return (p() & V_MASK) != 0; }
	bool flag_z() const { return (p() & Z_MASK) != 0; }

public:
	uint64_t cycle_count() const { return P6502::executed_cycles(); }
	void burn(int cycles)        { P6502::burn(cycles); }
	void clear_irq(IRQ_SOURCE source);
	void exec(int cycles);
	void irq(IRQ_SOURCE source);
	void nmi()                   { P6502::nmi(); }
	void schedule_spr_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count);
	void schedule_dmc_dma(P6502::dma_handler_t dma_handler, uint16_t source_address, uint16_t count);

public:
	// 2 k pages
	void swap_01(uint8_t *ptr) { page_[0x00] = ptr + 0x0000; page_[0x01] = ptr + 0x0800; page_[0x02] = ptr + 0x1000; page_[0x03] = ptr + 0x1800; }
	void swap_23(uint8_t *ptr) { page_[0x04] = ptr + 0x0000; page_[0x05] = ptr + 0x0800; page_[0x06] = ptr + 0x1000; page_[0x07] = ptr + 0x1800; }
	void swap_45(uint8_t *ptr) { page_[0x08] = ptr + 0x0000; page_[0x09] = ptr + 0x0800; page_[0x0a] = ptr + 0x1000; page_[0x0b] = ptr + 0x1800; }
	void swap_67(uint8_t *ptr) { page_[0x0c] = ptr + 0x0000; page_[0x0d] = ptr + 0x0800; page_[0x0e] = ptr + 0x1000; page_[0x0f] = ptr + 0x1800; }
	void swap_89(uint8_t *ptr) { page_[0x10] = ptr + 0x0000; page_[0x11] = ptr + 0x0800; page_[0x12] = ptr + 0x1000; page_[0x13] = ptr + 0x1800; }
	void swap_ab(uint8_t *ptr) { page_[0x14] = ptr + 0x0000; page_[0x15] = ptr + 0x0800; page_[0x16] = ptr + 0x1000; page_[0x17] = ptr + 0x1800; }
	void swap_cd(uint8_t *ptr) { page_[0x18] = ptr + 0x0000; page_[0x19] = ptr + 0x0800; page_[0x1a] = ptr + 0x1000; page_[0x1b] = ptr + 0x1800; }
	void swap_ef(uint8_t *ptr) { page_[0x1c] = ptr + 0x0000; page_[0x1d] = ptr + 0x0800; page_[0x1e] = ptr + 0x1000; page_[0x1f] = ptr + 0x1800; }

	void unmap_01() { page_[0x00] = 0; page_[0x01] = 0; page_[0x02] = 0; page_[0x03] = 0; }
	void unmap_23() { page_[0x04] = 0; page_[0x05] = 0; page_[0x06] = 0; page_[0x07] = 0; }
	void unmap_45() { page_[0x08] = 0; page_[0x09] = 0; page_[0x0a] = 0; page_[0x0b] = 0; }
	void unmap_67() { page_[0x0c] = 0; page_[0x0d] = 0; page_[0x0e] = 0; page_[0x0f] = 0; }
	void unmap_89() { page_[0x10] = 0; page_[0x11] = 0; page_[0x12] = 0; page_[0x13] = 0; }
	void unmap_ab() { page_[0x14] = 0; page_[0x15] = 0; page_[0x16] = 0; page_[0x17] = 0; }
	void unmap_cd() { page_[0x18] = 0; page_[0x19] = 0; page_[0x1a] = 0; page_[0x1b] = 0; }
	void unmap_ef() { page_[0x1c] = 0; page_[0x1d] = 0; page_[0x1e] = 0; page_[0x1f] = 0; }

public:
	void trash_ram();
	void reset(nes::RESET reset_type);

private:
	uint8_t *page_[page_count];
	uint8_t ram_[0x800];
	uint8_t irq_sources_;
};

#endif
