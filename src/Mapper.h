
#ifndef MAPPER_20080314_H_
#define MAPPER_20080314_H_

#include "Cpu.h"
#include "MemoryMappedFile.h"
#include "VRAMBank.h"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>

class Mapper;

using create_func = std::function<std::unique_ptr<Mapper>()>;

class Mapper {
public:
	static std::unique_ptr<Mapper> create_mapper(int num);
	static void register_mapper(int num, create_func create_ptr);

public:
	MemoryMappedFile open_sram(size_t size);

public:
	// the standard mapper interface
	virtual void save() {}
	virtual void restore() {}
	virtual std::string name() const = 0;

protected:
	Mapper();
	Mapper(const Mapper &) = delete;
	Mapper &operator=(const Mapper &) = delete;

public:
	virtual ~Mapper() = default;

public:
	// write routines
	virtual void write_0(uint16_t address, uint8_t value);
	virtual void write_1(uint16_t address, uint8_t value);
	virtual void write_2(uint16_t address, uint8_t value);
	virtual void write_3(uint16_t address, uint8_t value);
	virtual void write_4(uint16_t address, uint8_t value);
	virtual void write_5(uint16_t address, uint8_t value);
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

	// read routines
	virtual uint8_t read_0(uint16_t address);
	virtual uint8_t read_1(uint16_t address);
	virtual uint8_t read_2(uint16_t address);
	virtual uint8_t read_3(uint16_t address);
	virtual uint8_t read_4(uint16_t address);
	virtual uint8_t read_5(uint16_t address);
	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);
	virtual uint8_t read_8(uint16_t address);
	virtual uint8_t read_9(uint16_t address);
	virtual uint8_t read_a(uint16_t address);
	virtual uint8_t read_b(uint16_t address);
	virtual uint8_t read_c(uint16_t address);
	virtual uint8_t read_d(uint16_t address);
	virtual uint8_t read_e(uint16_t address);
	virtual uint8_t read_f(uint16_t address);

	// hooks for mappers to intercept VRAM/VROM read/writes
	virtual void write_vram(uint16_t address, uint8_t value);
	virtual uint8_t read_vram(uint16_t address);

	virtual void cpu_sync();
	virtual void ppu_end_frame();
	virtual void vram_change_hook(uint16_t vram_address);

protected:
	uint8_t read_memory(uint16_t address);

protected:
	// ---- PRG ROM ----
	// 8K swaps
	void set_prg_67(int num);
	void set_prg_89(int num);
	void set_prg_ab(int num);
	void set_prg_cd(int num);
	void set_prg_ef(int num);

	// 16K swaps
	void set_prg_89ab(int num);
	void set_prg_cdef(int num);

	// 32K swap
	void set_prg_89abcdef(int num);

	// ---- CHR ROM ----
	// 1K-swaps
	void set_chr_0000_03ff(int num);
	void set_chr_0400_07ff(int num);
	void set_chr_0800_0bff(int num);
	void set_chr_0c00_0fff(int num);
	void set_chr_1000_13ff(int num);
	void set_chr_1400_17ff(int num);
	void set_chr_1800_1bff(int num);
	void set_chr_1c00_1fff(int num);

	// 2K swaps
	void set_chr_0000_07ff(int num);
	void set_chr_0800_0fff(int num);
	void set_chr_1000_17ff(int num);
	void set_chr_1800_1fff(int num);

	// 4K swaps
	void set_chr_0000_0fff(int num);
	void set_chr_1000_1fff(int num);

	// 8K swaps
	void set_chr_0000_1fff(int num);

	// ---- CHR RAM ----
	// 1K-swaps
	void set_chr_0000_03ff_ram(uint8_t *p, int num);
	void set_chr_0400_07ff_ram(uint8_t *p, int num);
	void set_chr_0800_0bff_ram(uint8_t *p, int num);
	void set_chr_0c00_0fff_ram(uint8_t *p, int num);
	void set_chr_1000_13ff_ram(uint8_t *p, int num);
	void set_chr_1400_17ff_ram(uint8_t *p, int num);
	void set_chr_1800_1bff_ram(uint8_t *p, int num);
	void set_chr_1c00_1fff_ram(uint8_t *p, int num);

	// 2K swaps
	void set_chr_0000_07ff_ram(uint8_t *p, int num);
	void set_chr_0800_0fff_ram(uint8_t *p, int num);
	void set_chr_1000_17ff_ram(uint8_t *p, int num);
	void set_chr_1800_1fff_ram(uint8_t *p, int num);

	// 4K swaps
	void set_chr_0000_0fff_ram(uint8_t *p, int num);
	void set_chr_1000_1fff_ram(uint8_t *p, int num);

	// 8K swaps
	void set_chr_0000_1fff_ram(uint8_t *p, int num);

protected:
	void set_mirroring(uint8_t mir);

protected:
	void swap_01(uint8_t *ptr) {
		page_[0x00] = ptr + 0x0000;
		page_[0x01] = ptr + 0x0800;
		page_[0x02] = ptr + 0x1000;
		page_[0x03] = ptr + 0x1800;
	}
	void swap_23(uint8_t *ptr) {
		page_[0x04] = ptr + 0x0000;
		page_[0x05] = ptr + 0x0800;
		page_[0x06] = ptr + 0x1000;
		page_[0x07] = ptr + 0x1800;
	}
	void swap_45(uint8_t *ptr) {
		page_[0x08] = ptr + 0x0000;
		page_[0x09] = ptr + 0x0800;
		page_[0x0a] = ptr + 0x1000;
		page_[0x0b] = ptr + 0x1800;
	}
	void swap_67(uint8_t *ptr) {
		page_[0x0c] = ptr + 0x0000;
		page_[0x0d] = ptr + 0x0800;
		page_[0x0e] = ptr + 0x1000;
		page_[0x0f] = ptr + 0x1800;
	}
	void swap_89(uint8_t *ptr) {
		page_[0x10] = ptr + 0x0000;
		page_[0x11] = ptr + 0x0800;
		page_[0x12] = ptr + 0x1000;
		page_[0x13] = ptr + 0x1800;
	}
	void swap_ab(uint8_t *ptr) {
		page_[0x14] = ptr + 0x0000;
		page_[0x15] = ptr + 0x0800;
		page_[0x16] = ptr + 0x1000;
		page_[0x17] = ptr + 0x1800;
	}
	void swap_cd(uint8_t *ptr) {
		page_[0x18] = ptr + 0x0000;
		page_[0x19] = ptr + 0x0800;
		page_[0x1a] = ptr + 0x1000;
		page_[0x1b] = ptr + 0x1800;
	}
	void swap_ef(uint8_t *ptr) {
		page_[0x1c] = ptr + 0x0000;
		page_[0x1d] = ptr + 0x0800;
		page_[0x1e] = ptr + 0x1000;
		page_[0x1f] = ptr + 0x1800;
	}

	void unmap_01() {
		page_[0x00] = nullptr;
		page_[0x01] = nullptr;
		page_[0x02] = nullptr;
		page_[0x03] = nullptr;
	}
	void unmap_23() {
		page_[0x04] = nullptr;
		page_[0x05] = nullptr;
		page_[0x06] = nullptr;
		page_[0x07] = nullptr;
	}
	void unmap_45() {
		page_[0x08] = nullptr;
		page_[0x09] = nullptr;
		page_[0x0a] = nullptr;
		page_[0x0b] = nullptr;
	}
	void unmap_67() {
		page_[0x0c] = nullptr;
		page_[0x0d] = nullptr;
		page_[0x0e] = nullptr;
		page_[0x0f] = nullptr;
	}
	void unmap_89() {
		page_[0x10] = nullptr;
		page_[0x11] = nullptr;
		page_[0x12] = nullptr;
		page_[0x13] = nullptr;
	}
	void unmap_ab() {
		page_[0x14] = nullptr;
		page_[0x15] = nullptr;
		page_[0x16] = nullptr;
		page_[0x17] = nullptr;
	}
	void unmap_cd() {
		page_[0x18] = nullptr;
		page_[0x19] = nullptr;
		page_[0x1a] = nullptr;
		page_[0x1b] = nullptr;
	}
	void unmap_ef() {
		page_[0x1c] = nullptr;
		page_[0x1d] = nullptr;
		page_[0x1e] = nullptr;
		page_[0x1f] = nullptr;
	}

protected:
	constexpr static uint8_t mirror_single_low  = 0x00;
	constexpr static uint8_t mirror_single_high = 0x55;
	constexpr static uint8_t mirror_vertical    = 0x44;
	constexpr static uint8_t mirror_horizontal  = 0x50;
	constexpr static uint8_t mirror_4screen     = 0xe4;

private:
	constexpr static unsigned int PageCount = 32;
	constexpr static unsigned int PageSize  = 0x10000 / PageCount;
	constexpr static unsigned int PageMask  = PageSize - 1;
	constexpr static unsigned int PageShift = 11;

private:
	uint8_t nametables_[4 * 0x400] = {}; // nametable and attribute table data
										 // 4 nametables, each $03c0 bytes in
										 // size plus 4 corresponding $40 byte
										 // attribute tables
										 // even though the real thing only has 2
										 // tables, we currently simulate 4 for
										 // simplicity
	VRAMBank vram_banks_[0x10] = {};

	uint8_t *page_[PageCount] = {};

private:
	// we use a function here to return a static object
	// to ensure order of initialization, otherwise, the mappers
	// could try to register themselves before the map was constructed
	static std::map<int, create_func> &registered_mappers_ines() {
		static std::map<int, create_func> static_map;
		return static_map;
	}
};

// template which is a class whose constructor adds a function to our mapper
// creation table in order to create an object of each type of mapper
template <class T>
struct MapperRegisterObject {
	explicit MapperRegisterObject(int n) {
		Mapper::register_mapper(n, []() {
			return std::make_unique<T>();
		});
	}
};

// macro to hide details of setting up a static instance of the previously
// mentioned class. The first line of each Mapper[NNN].cpp file should be
// a call to this macro passing the appropriate mapper number
#define SETUP_STATIC_INES_MAPPER_REGISTRAR(N)                 \
	namespace {                                               \
	static MapperRegisterObject<Mapper##N> MapperRegister(N); \
	}

#endif
