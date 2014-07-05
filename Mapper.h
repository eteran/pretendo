
#ifndef MAPPER_20080314_H_
#define MAPPER_20080314_H_

#include "CPU.h"
#include "Cart.h"
#include <cstdint>
#include <map>
#include <memory>
#include <string>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

class Mapper;

typedef std::shared_ptr<Mapper> (*create_ptr)();

class Mapper {
public:
	static std::shared_ptr<Mapper> create_mapper(int num);
	static void register_mapper(int num, create_ptr create_ptr);

	void write_memory(uint16_t address, uint8_t value);
	uint8_t read_memory(uint16_t address);

	void hsync();

public:
	// the standard mapper interface
	virtual void save()		{};
	virtual void restore()	{};
	virtual std::string name() const = 0;

protected:
	Mapper();

private:
	Mapper(const Mapper &) = delete;
	Mapper &operator=(const Mapper &) = delete;

public:
	virtual ~Mapper();

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
	// ---- PRG ROM ----
	// 8K swaps
	void set_prg_67(int num) const;
	void set_prg_89(int num) const;
	void set_prg_ab(int num) const;
	void set_prg_cd(int num) const;
	void set_prg_ef(int num) const;

	// 16K swaps
	void set_prg_89ab(int num) const;
	void set_prg_cdef(int num) const;

	// TODO: remove the need for this, handle it in the Mapper read/write hooks
	void set_prg_89ab_ptr(uint8_t *p) const { nes::cpu::swap_89(p + 0x0000); nes::cpu::swap_ab(p + 0x2000); }
	void set_prg_cdef_ptr(uint8_t *p) const { nes::cpu::swap_cd(p + 0x0000); nes::cpu::swap_ef(p + 0x2000); }

	// 32K swap
	void set_prg_89abcdef(int num) const;

	// TODO: remove the need for this, handle it in the Mapper read/write hooks
	void set_prg_89abcdef_ptr(uint8_t *p) const {
		nes::cpu::swap_89(p + 0x0000);
		nes::cpu::swap_ab(p + 0x2000);
		nes::cpu::swap_cd(p + 0x4000);
		nes::cpu::swap_ef(p + 0x6000);
	}

	// ---- CHR ROM ----
	// 1K-swaps
	void set_chr_0000_03ff(int num) const;
	void set_chr_0400_07ff(int num) const;
	void set_chr_0800_0bff(int num) const;
	void set_chr_0c00_0fff(int num) const;
	void set_chr_1000_13ff(int num) const;
	void set_chr_1400_17ff(int num) const;
	void set_chr_1800_1bff(int num) const;
	void set_chr_1c00_1fff(int num) const;

	// 2K swaps
	void set_chr_0000_07ff(int num) const;
	void set_chr_0800_0fff(int num) const;
	void set_chr_1000_17ff(int num) const;
	void set_chr_1800_1fff(int num) const;

	// 4K swaps
	void set_chr_0000_0fff(int num) const;
	void set_chr_1000_1fff(int num) const;

	// 8K swaps
	void set_chr_0000_1fff(int num) const;

	// ---- CHR RAM ----
	// 1K-swaps
	void set_chr_0000_03ff_ram(uint8_t *p, int num) const;
	void set_chr_0400_07ff_ram(uint8_t *p, int num) const;
	void set_chr_0800_0bff_ram(uint8_t *p, int num) const;
	void set_chr_0c00_0fff_ram(uint8_t *p, int num) const;
	void set_chr_1000_13ff_ram(uint8_t *p, int num) const;
	void set_chr_1400_17ff_ram(uint8_t *p, int num) const;
	void set_chr_1800_1bff_ram(uint8_t *p, int num) const;
	void set_chr_1c00_1fff_ram(uint8_t *p, int num) const;

	// 2K swaps
	void set_chr_0000_07ff_ram(uint8_t *p, int num) const;
	void set_chr_0800_0fff_ram(uint8_t *p, int num) const;
	void set_chr_1000_17ff_ram(uint8_t *p, int num) const;
	void set_chr_1800_1fff_ram(uint8_t *p, int num) const;

	// 4K swaps
	void set_chr_0000_0fff_ram(uint8_t *p, int num) const;
	void set_chr_1000_1fff_ram(uint8_t *p, int num) const;

	// 8K swaps
	void set_chr_0000_1fff_ram(uint8_t *p, int num) const;

private:
	// we use a function here to return a static object
	// to ensure order of initialization, otherwise, the mappers
	// could try to register themselves before the map was constructed
	static std::map<int, create_ptr> &registered_mappers_ines() {
		static std::map<int, create_ptr> static_map;
		return static_map;
	}
};


// template which is a class whose constructor adds a function to our mapper
// creation table in order to create an object of each type of mapper
template<class T>
struct MapperRegisterObject {
	MapperRegisterObject(int n) {
		Mapper::register_mapper(n, MapperCreate);
	}

	static std::shared_ptr<Mapper> MapperCreate() {
		return std::shared_ptr<Mapper>(new T());
	}
};


// macro to hide details of setting up a static instance of the previously
// mentioned macro. The first line of each Mapper[NNN].cpp file should be
// a call to this macro passing the appropriate mapper number
// NOTE: in order for the __COUNTER__ macro expansion to work,
// we need the middle-man function "FUNC1" to call "FUNC2"

#define SETUP_STATIC_MAPPER_REGISTRAR_FUNC2(X, U, Y) static MapperRegisterObject<Mapper##X> Mapper##U##Register(Y)
#define SETUP_STATIC_MAPPER_REGISTRAR_FUNC1(X, U, Y) SETUP_STATIC_MAPPER_REGISTRAR_FUNC2(X, U, Y)
#define SETUP_STATIC_INES_MAPPER_REGISTRAR(N)        SETUP_STATIC_MAPPER_REGISTRAR_FUNC1(N, __COUNTER__, N)

#endif

