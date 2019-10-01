
#include "Mapper.h"
#include "APU.h"
#include "CPU.h"
#include "Cart.h"
#include "Input.h"
#include "Config.h"
#include "NES.h"
#include "PPU.h"
#include "Compiler.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#ifdef __linux__
#include <pwd.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


//------------------------------------------------------------------------------
// Name: Mapper
//------------------------------------------------------------------------------
Mapper::Mapper() {

	// NOTE: we're not supporting trainers anymore, that's why the code is gone

	std::fill(std::begin(nametables_), std::end(nametables_), 0);
	std::fill(std::begin(page_), std::end(page_), nullptr);

	switch(nes::cart.mirroring()) {
	case Cart::MIR_VERTICAL:    set_mirroring(nes::ppu::mirror_vertical);    break;
	case Cart::MIR_HORIZONTAL:  set_mirroring(nes::ppu::mirror_horizontal);  break;
	case Cart::MIR_SINGLE_LOW:  set_mirroring(nes::ppu::mirror_single_low);  break;
	case Cart::MIR_SINGLE_HIGH: set_mirroring(nes::ppu::mirror_single_high); break;
	case Cart::MIR_4SCREEN:     set_mirroring(nes::ppu::mirror_4screen);     break;
	case Cart::MIR_MAPPER:
	default:
		// Nothing, handled by Mapper
		break;
	}


}

//------------------------------------------------------------------------------
// Name: create_mapper
//------------------------------------------------------------------------------
std::unique_ptr<Mapper> Mapper::create_mapper(int num) {

	create_func f = nullptr;

	const std::map<int, create_func> &mappers = registered_mappers_ines();
	auto it = mappers.find(num);
	if(it != mappers.end() && (f = it->second)) {
		auto ret = f();
		std::cout << "[Mapper::create_mapper] mapper #" << num << " loaded, type: " << ret->name() << std::endl;
		return ret;
	} else {
		std::cout << "unsupported mapper hardware - iNES number: " << num << std::endl;
		return nullptr;
	}
}

//------------------------------------------------------------------------------
// Name: write_memory
//------------------------------------------------------------------------------
void Mapper::write_memory(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: read_memory
//------------------------------------------------------------------------------
uint8_t Mapper::read_memory(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: write_0
//------------------------------------------------------------------------------
void Mapper::write_0(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_1
//------------------------------------------------------------------------------
void Mapper::write_1(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_2
//------------------------------------------------------------------------------
void Mapper::write_2(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_3
//------------------------------------------------------------------------------
void Mapper::write_3(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: Write4
//------------------------------------------------------------------------------
void Mapper::write_4(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_5
//------------------------------------------------------------------------------
void Mapper::write_5(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_6
//------------------------------------------------------------------------------
void Mapper::write_6(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_7
//------------------------------------------------------------------------------
void Mapper::write_7(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_8
//------------------------------------------------------------------------------
void Mapper::write_8(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_9
//------------------------------------------------------------------------------
void Mapper::write_9(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void Mapper::write_a(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Mapper::write_b(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Mapper::write_c(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Mapper::write_d(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Mapper::write_e(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void Mapper::write_f(uint16_t address, uint8_t value) {
	write_memory(address, value);
}

//------------------------------------------------------------------------------
// Name: read_0
//------------------------------------------------------------------------------
uint8_t Mapper::read_0(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_1
//------------------------------------------------------------------------------
uint8_t Mapper::read_1(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_2
//------------------------------------------------------------------------------
uint8_t Mapper::read_2(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_3
//------------------------------------------------------------------------------
uint8_t Mapper::read_3(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_4
//------------------------------------------------------------------------------
uint8_t Mapper::read_4(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_5
//------------------------------------------------------------------------------
uint8_t Mapper::read_5(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_6
//------------------------------------------------------------------------------
uint8_t Mapper::read_6(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_7
//------------------------------------------------------------------------------
uint8_t Mapper::read_7(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_8
//------------------------------------------------------------------------------
uint8_t Mapper::read_8(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_9
//------------------------------------------------------------------------------
uint8_t Mapper::read_9(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_a
//------------------------------------------------------------------------------
uint8_t Mapper::read_a(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_b
//------------------------------------------------------------------------------
uint8_t Mapper::read_b(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_c
//------------------------------------------------------------------------------
uint8_t Mapper::read_c(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_d
//------------------------------------------------------------------------------
uint8_t Mapper::read_d(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_e
//------------------------------------------------------------------------------
uint8_t Mapper::read_e(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name: read_f
//------------------------------------------------------------------------------
uint8_t Mapper::read_f(uint16_t address) {
	return read_memory(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::write_vram(uint16_t address, uint8_t value) {
	VRAMBank &bank = vram_banks_[(address >> 10) & 0x0f];
	if(LIKELY(bank && bank.writeable())) {
		bank[address & 0x03ff] = value;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper::read_vram(uint16_t address) {
	const VRAMBank &bank = vram_banks_[(address >> 10) & 0x0f];
	if(LIKELY(bank)) {
		return bank[address & 0x03ff];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: set_prg_67
//------------------------------------------------------------------------------
void Mapper::set_prg_67(int num) {
	num *= (8 * 1024);
	const uint32_t mask = nes::cart.prg_mask();
	swap_67(nes::cart.prg() + (num & mask));
}

//------------------------------------------------------------------------------
// Name: set_prg_89
//------------------------------------------------------------------------------
void Mapper::set_prg_89(int num) {
	num *= (8 * 1024);
	const uint32_t mask = nes::cart.prg_mask();
	swap_89(nes::cart.prg() + (num & mask));
}

//------------------------------------------------------------------------------
// Name: set_prg_ab
//------------------------------------------------------------------------------
void Mapper::set_prg_ab(int num) {
	num *= (8 * 1024);
	const uint32_t mask = nes::cart.prg_mask();
	swap_ab(nes::cart.prg() + (num & mask));
}

//------------------------------------------------------------------------------
// Name: set_prg_cd
//------------------------------------------------------------------------------
void Mapper::set_prg_cd(int num) {
	num *= (8 * 1024);
	const uint32_t mask = nes::cart.prg_mask();
	swap_cd(nes::cart.prg() + (num & mask));
}

//------------------------------------------------------------------------------
// Name: set_prg_ef
//------------------------------------------------------------------------------
void Mapper::set_prg_ef(int num) {
	num *= (8 * 1024);
	const uint32_t mask = nes::cart.prg_mask();
	swap_ef(nes::cart.prg() + (num & mask));
}

//------------------------------------------------------------------------------
// Name: set_prg_89ab
//------------------------------------------------------------------------------
void Mapper::set_prg_89ab(int num) {
	num *= (16 * 1024);
	const uint32_t mask = nes::cart.prg_mask();
	swap_89(nes::cart.prg() + ((num + 0x0000) & mask));
	swap_ab(nes::cart.prg() + ((num + 0x2000) & mask));
}

//------------------------------------------------------------------------------
// Name: set_prg_cdef
//------------------------------------------------------------------------------
void Mapper::set_prg_cdef(int num) {
	num *= (16 * 1024);
	const uint32_t mask = nes::cart.prg_mask();
	swap_cd(nes::cart.prg() + ((num + 0x0000) & mask));
	swap_ef(nes::cart.prg() + ((num + 0x2000) & mask));
}

//------------------------------------------------------------------------------
// Name: set_prg_89abcdef
//------------------------------------------------------------------------------
void Mapper::set_prg_89abcdef(int num) {
	num *= (32 * 1024);
	const uint32_t mask = nes::cart.prg_mask();
	swap_89(nes::cart.prg() + ((num + 0x0000) & mask));
	swap_ab(nes::cart.prg() + ((num + 0x2000) & mask));
	swap_cd(nes::cart.prg() + ((num + 0x4000) & mask));
	swap_ef(nes::cart.prg() + ((num + 0x6000) & mask));
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_03ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_03ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0400_07ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0400_07ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0000) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0800_0bff
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0bff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0000) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0c00_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0c00_0fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0000) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_13ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_13ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1400_17ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1400_17ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0000) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1800_1bff
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1bff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0000) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1c00_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1c00_1fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0000) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_07ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_07ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & mask), false);
		set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0800_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0000) & mask), false);
		set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0400) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_17ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_17ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & mask), false);
		set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0400) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1800_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0000) & mask), false);
		set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0400) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_0fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (4 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & mask), false);
		set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & mask), false);
		set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0800) & mask), false);
		set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0c00) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_1fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (4 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & mask), false);
		set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0400) & mask), false);
		set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0800) & mask), false);
		set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0c00) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_1fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (8 * 1024);
		const uint32_t mask = nes::cart.chr_mask();
		set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & mask), false);
		set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & mask), false);
		set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0800) & mask), false);
		set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0c00) & mask), false);
		set_vram_bank(0x04, nes::cart.chr() + ((num + 0x1000) & mask), false);
		set_vram_bank(0x05, nes::cart.chr() + ((num + 0x1400) & mask), false);
		set_vram_bank(0x06, nes::cart.chr() + ((num + 0x1800) & mask), false);
		set_vram_bank(0x07, nes::cart.chr() + ((num + 0x1c00) & mask), false);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_03ff_ram(uint8_t *p, int num) {
	num *= (1 * 1024);
	set_vram_bank(0x00, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0400_07ff_ram(uint8_t *p, int num) {
	num *= (1 * 1024);
	set_vram_bank(0x01, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0bff_ram(uint8_t *p, int num) {
	num *= (1 * 1024);
	set_vram_bank(0x02, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0c00_0fff_ram(uint8_t *p, int num) {
	num *= (1 * 1024);
	set_vram_bank(0x03, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_13ff_ram(uint8_t *p, int num) {
	num *= (1 * 1024);
	set_vram_bank(0x04, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1400_17ff_ram(uint8_t *p, int num) {
	num *= (1 * 1024);
	set_vram_bank(0x05, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1bff_ram(uint8_t *p, int num) {
	num *= (1 * 1024);
	set_vram_bank(0x06, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1c00_1fff_ram(uint8_t *p, int num) {
	num *= (1 * 1024);
	set_vram_bank(0x07, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_07ff_ram(uint8_t *p, int num) {
	num *= (2 * 1024);
	set_vram_bank(0x00, p + num + 0x0000, true);
	set_vram_bank(0x01, p + num + 0x0400, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0fff_ram(uint8_t *p, int num) {
	num *= (2 * 1024);
	set_vram_bank(0x02, p + num + 0x0000, true);
	set_vram_bank(0x03, p + num + 0x0400, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_17ff_ram(uint8_t *p, int num) {
	num *= (2 * 1024);
	set_vram_bank(0x04, p + num + 0x0000, true);
	set_vram_bank(0x05, p + num + 0x0400, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1fff_ram(uint8_t *p, int num) {
	num *= (2 * 1024);
	set_vram_bank(0x06, p + num + 0x0000, true);
	set_vram_bank(0x07, p + num + 0x0400, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_0fff_ram(uint8_t *p, int num) {
	num *= (4 * 1024);
	set_vram_bank(0x00, p + num + 0x0000, true);
	set_vram_bank(0x01, p + num + 0x0400, true);
	set_vram_bank(0x02, p + num + 0x0800, true);
	set_vram_bank(0x03, p + num + 0x0c00, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_1fff_ram(uint8_t *p, int num) {
	num *= (4 * 1024);
	set_vram_bank(0x04, p + num + 0x0000, true);
	set_vram_bank(0x05, p + num + 0x0400, true);
	set_vram_bank(0x06, p + num + 0x0800, true);
	set_vram_bank(0x07, p + num + 0x0c00, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_1fff_ram(uint8_t *p, int num) {
	num *= (8 * 1024);
	set_vram_bank(0x00, p + num + 0x0000, true);
	set_vram_bank(0x01, p + num + 0x0400, true);
	set_vram_bank(0x02, p + num + 0x0800, true);
	set_vram_bank(0x03, p + num + 0x0c00, true);
	set_vram_bank(0x04, p + num + 0x1000, true);
	set_vram_bank(0x05, p + num + 0x1400, true);
	set_vram_bank(0x06, p + num + 0x1800, true);
	set_vram_bank(0x07, p + num + 0x1c00, true);
}

//------------------------------------------------------------------------------
// Name: set_mirroring
//------------------------------------------------------------------------------
void Mapper::set_mirroring(uint8_t mir) {
	// utilizes the concept of a mirroring control byte
	// each pair of bits represents a table to be mirrored from
	// bits 0/1 are for bank 8/c
	// bits 2/3 are for bank 9/d
	// bits 4/5 are for bank a/e
	// bits 6/7 are for bank b/f

	// so:
	// vertical:   01000100b, or $44
	// horizontal: 01010000b, or $50
	// 4 screen:   11100100b, or $e4
	// single lo:  00000000b, or $00
	// single hi:  01010101b, or $55

	vram_banks_[0x08] = { &nametables_[(mir << 0xa) & 0x0c00], true };
	vram_banks_[0x09] = { &nametables_[(mir << 0x8) & 0x0c00], true };
	vram_banks_[0x0a] = { &nametables_[(mir << 0x6) & 0x0c00], true };
	vram_banks_[0x0b] = { &nametables_[(mir << 0x4) & 0x0c00], true };

	// [$3000, $4000) mirrors [$2000, $3000)
	vram_banks_[0x0c] = { &nametables_[(mir << 0xa) & 0x0c00], true };
	vram_banks_[0x0d] = { &nametables_[(mir << 0x8) & 0x0c00], true };
	vram_banks_[0x0e] = { &nametables_[(mir << 0x6) & 0x0c00], true };
	vram_banks_[0x0f] = { &nametables_[(mir << 0x4) & 0x0c00], true };
}

//------------------------------------------------------------------------------
// Name: set_vram_bank
//------------------------------------------------------------------------------
void Mapper::set_vram_bank(uint8_t bank, uint8_t *p, bool writeable) {
	assert(p);
	vram_banks_[bank] = { p, writeable };
}

//------------------------------------------------------------------------------
// Name: cpu_sync
//------------------------------------------------------------------------------
void Mapper::cpu_sync() {
	// default does nothing
}

//------------------------------------------------------------------------------
// Name: ppu_end_frame
//------------------------------------------------------------------------------
void Mapper::ppu_end_frame() {
	// default does nothing
}

//------------------------------------------------------------------------------
// Name: vram_change_hook
//------------------------------------------------------------------------------
void Mapper::vram_change_hook(uint16_t vram_address) {
	(void)vram_address;
	// default does nothing
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::register_mapper(int num, create_func create_ptr) {
    assert(create_ptr);

	std::cout << "[Mapper::register_mapper] Registering Mapper: " << num << std::endl;
	registered_mappers_ines()[num] = create_ptr;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
MemoryMappedFile Mapper::open_sram(size_t size) {

    const boost::filesystem::path cache_path = Config::cacheDirectory();
    boost::filesystem::create_directories(cache_path);

    char hex_buf[32];
    snprintf(hex_buf, sizeof(hex_buf), "%08x", nes::cart.rom_hash());
    boost::filesystem::path save_file = cache_path / (std::string(hex_buf) + ".sav");

    return MemoryMappedFile(save_file.string(), size);
}
