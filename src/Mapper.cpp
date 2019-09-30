
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

	switch(nes::cart.mirroring()) {
	case Cart::MIR_VERTICAL:    nes::ppu::set_mirroring(nes::ppu::mirror_vertical);    break;
	case Cart::MIR_HORIZONTAL:  nes::ppu::set_mirroring(nes::ppu::mirror_horizontal);  break;
	case Cart::MIR_SINGLE_LOW:  nes::ppu::set_mirroring(nes::ppu::mirror_single_low);  break;
	case Cart::MIR_SINGLE_HIGH: nes::ppu::set_mirroring(nes::ppu::mirror_single_high); break;
	case Cart::MIR_4SCREEN:     nes::ppu::set_mirroring(nes::ppu::mirror_4screen);     break;
	case Cart::MIR_MAPPER:
	default:
		// Nothing, handled by Mapper
		break;
	}
}

//------------------------------------------------------------------------------
// Name: ~Mapper()
//------------------------------------------------------------------------------
Mapper::~Mapper() {
	nes::ppu::unset_vram_bank(0);
	nes::ppu::unset_vram_bank(1);
	nes::ppu::unset_vram_bank(2);
	nes::ppu::unset_vram_bank(3);
	nes::ppu::unset_vram_bank(4);
	nes::ppu::unset_vram_bank(5);
	nes::ppu::unset_vram_bank(6);
	nes::ppu::unset_vram_bank(7);
	
	nes::ppu::unset_vram_bank(8);
	nes::ppu::unset_vram_bank(9);
	nes::ppu::unset_vram_bank(10);
	nes::ppu::unset_vram_bank(11);
	nes::ppu::unset_vram_bank(12);
	nes::ppu::unset_vram_bank(13);
	nes::ppu::unset_vram_bank(14);
	nes::ppu::unset_vram_bank(15);
}

//------------------------------------------------------------------------------
// Name: create_mapper
//------------------------------------------------------------------------------
std::shared_ptr<Mapper> Mapper::create_mapper(int num) {

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
// Name: write_0
//------------------------------------------------------------------------------
void Mapper::write_0(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_1
//------------------------------------------------------------------------------
void Mapper::write_1(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_2
//------------------------------------------------------------------------------
void Mapper::write_2(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_3
//------------------------------------------------------------------------------
void Mapper::write_3(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: Write4
//------------------------------------------------------------------------------
void Mapper::write_4(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_5
//------------------------------------------------------------------------------
void Mapper::write_5(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_6
//------------------------------------------------------------------------------
void Mapper::write_6(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_7
//------------------------------------------------------------------------------
void Mapper::write_7(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_8
//------------------------------------------------------------------------------
void Mapper::write_8(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_9
//------------------------------------------------------------------------------
void Mapper::write_9(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void Mapper::write_a(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Mapper::write_b(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Mapper::write_c(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Mapper::write_d(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Mapper::write_e(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void Mapper::write_f(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name: read_4
//------------------------------------------------------------------------------
uint8_t Mapper::read_4(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_5
//------------------------------------------------------------------------------
uint8_t Mapper::read_5(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_6
//------------------------------------------------------------------------------
uint8_t Mapper::read_6(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_7
//------------------------------------------------------------------------------
uint8_t Mapper::read_7(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_8
//------------------------------------------------------------------------------
uint8_t Mapper::read_8(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_9
//------------------------------------------------------------------------------
uint8_t Mapper::read_9(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_a
//------------------------------------------------------------------------------
uint8_t Mapper::read_a(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_b
//------------------------------------------------------------------------------
uint8_t Mapper::read_b(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_c
//------------------------------------------------------------------------------
uint8_t Mapper::read_c(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_d
//------------------------------------------------------------------------------
uint8_t Mapper::read_d(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_e
//------------------------------------------------------------------------------
uint8_t Mapper::read_e(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name: read_f
//------------------------------------------------------------------------------
uint8_t Mapper::read_f(uint16_t address) {
	if(LIKELY(page_[address >> page_shift])) {
		return page_[address >> page_shift][address & page_mask];
	}

	// simulate open bus
	return (address >> 8) & 0xff;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::write_vram(uint16_t address, uint8_t value) {
	nes::ppu::write_vram(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper::read_vram(uint16_t address) {
	return nes::ppu::read_vram(address);
}

//------------------------------------------------------------------------------
// Name: set_prg_67
//------------------------------------------------------------------------------
void Mapper::set_prg_67(int num) {
	num *= (8 * 1024);
	swap_67(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_89
//------------------------------------------------------------------------------
void Mapper::set_prg_89(int num) {
	num *= (8 * 1024);
	swap_89(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_ab
//------------------------------------------------------------------------------
void Mapper::set_prg_ab(int num) {
	num *= (8 * 1024);
	swap_ab(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_cd
//------------------------------------------------------------------------------
void Mapper::set_prg_cd(int num) {
	num *= (8 * 1024);
	swap_cd(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_ef
//------------------------------------------------------------------------------
void Mapper::set_prg_ef(int num) {
	num *= (8 * 1024);
	swap_ef(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_89ab
//------------------------------------------------------------------------------
void Mapper::set_prg_89ab(int num) {
	num *= (16 * 1024);
	swap_89(nes::cart.prg() + ((num + 0x0000) & nes::cart.prg_mask()));
	swap_ab(nes::cart.prg() + ((num + 0x2000) & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_cdef
//------------------------------------------------------------------------------
void Mapper::set_prg_cdef(int num) {
	num *= (16 * 1024);
	swap_cd(nes::cart.prg() + ((num + 0x0000) & nes::cart.prg_mask()));
	swap_ef(nes::cart.prg() + ((num + 0x2000) & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_89abcdef
//------------------------------------------------------------------------------
void Mapper::set_prg_89abcdef(int num) {
	num *= (32 * 1024);
	swap_89(nes::cart.prg() + ((num + 0x0000) & nes::cart.prg_mask()));
	swap_ab(nes::cart.prg() + ((num + 0x2000) & nes::cart.prg_mask()));
	swap_cd(nes::cart.prg() + ((num + 0x4000) & nes::cart.prg_mask()));
	swap_ef(nes::cart.prg() + ((num + 0x6000) & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_03ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_03ff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		nes::ppu::set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0400_07ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0400_07ff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		nes::ppu::set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0800_0bff
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0bff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		nes::ppu::set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0c00_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0c00_0fff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		nes::ppu::set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_13ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_13ff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		nes::ppu::set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1400_17ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1400_17ff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		nes::ppu::set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1800_1bff
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1bff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		nes::ppu::set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1c00_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1c00_1fff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		nes::ppu::set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_07ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_07ff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		nes::ppu::set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0800_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0fff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		nes::ppu::set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_17ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_17ff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		nes::ppu::set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1800_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1fff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		nes::ppu::set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_0fff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (4 * 1024);
		nes::ppu::set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0800) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0c00) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_1fff(int num) const {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (4 * 1024);
		nes::ppu::set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0800) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0c00) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_1fff(int num) const {	
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (8 * 1024);
		nes::ppu::set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0800) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0c00) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x04, nes::cart.chr() + ((num + 0x1000) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x05, nes::cart.chr() + ((num + 0x1400) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x06, nes::cart.chr() + ((num + 0x1800) & nes::cart.chr_mask()), false);
		nes::ppu::set_vram_bank(0x07, nes::cart.chr() + ((num + 0x1c00) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_03ff_ram(uint8_t *p, int num) const {
	num *= (1 * 1024);
	nes::ppu::set_vram_bank(0x00, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0400_07ff_ram(uint8_t *p, int num) const {
	num *= (1 * 1024);
	nes::ppu::set_vram_bank(0x01, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0bff_ram(uint8_t *p, int num) const {
	num *= (1 * 1024);
	nes::ppu::set_vram_bank(0x02, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0c00_0fff_ram(uint8_t *p, int num) const {
	num *= (1 * 1024);
	nes::ppu::set_vram_bank(0x03, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_13ff_ram(uint8_t *p, int num) const {
	num *= (1 * 1024);
	nes::ppu::set_vram_bank(0x04, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1400_17ff_ram(uint8_t *p, int num) const {
	num *= (1 * 1024);
	nes::ppu::set_vram_bank(0x05, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1bff_ram(uint8_t *p, int num) const {
	num *= (1 * 1024);
	nes::ppu::set_vram_bank(0x06, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1c00_1fff_ram(uint8_t *p, int num) const {
	num *= (1 * 1024);
	nes::ppu::set_vram_bank(0x07, p + num + 0x0000, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_07ff_ram(uint8_t *p, int num) const {
	num *= (2 * 1024);
	nes::ppu::set_vram_bank(0x00, p + num + 0x0000, true);
	nes::ppu::set_vram_bank(0x01, p + num + 0x0400, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0fff_ram(uint8_t *p, int num) const {
	num *= (2 * 1024);
	nes::ppu::set_vram_bank(0x02, p + num + 0x0000, true);
	nes::ppu::set_vram_bank(0x03, p + num + 0x0400, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_17ff_ram(uint8_t *p, int num) const {
	num *= (2 * 1024);
	nes::ppu::set_vram_bank(0x04, p + num + 0x0000, true);
	nes::ppu::set_vram_bank(0x05, p + num + 0x0400, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1fff_ram(uint8_t *p, int num) const {
	num *= (2 * 1024);
	nes::ppu::set_vram_bank(0x06, p + num + 0x0000, true);
	nes::ppu::set_vram_bank(0x07, p + num + 0x0400, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_0fff_ram(uint8_t *p, int num) const {
	num *= (4 * 1024);
	nes::ppu::set_vram_bank(0x00, p + num + 0x0000, true);
	nes::ppu::set_vram_bank(0x01, p + num + 0x0400, true);
	nes::ppu::set_vram_bank(0x02, p + num + 0x0800, true);
	nes::ppu::set_vram_bank(0x03, p + num + 0x0c00, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_1fff_ram(uint8_t *p, int num) const {
	num *= (4 * 1024);
	nes::ppu::set_vram_bank(0x04, p + num + 0x0000, true);
	nes::ppu::set_vram_bank(0x05, p + num + 0x0400, true);
	nes::ppu::set_vram_bank(0x06, p + num + 0x0800, true);
	nes::ppu::set_vram_bank(0x07, p + num + 0x0c00, true);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_1fff_ram(uint8_t *p, int num) const {
	num *= (8 * 1024);
	nes::ppu::set_vram_bank(0x00, p + num + 0x0000, true);
	nes::ppu::set_vram_bank(0x01, p + num + 0x0400, true);
	nes::ppu::set_vram_bank(0x02, p + num + 0x0800, true);
	nes::ppu::set_vram_bank(0x03, p + num + 0x0c00, true);
	nes::ppu::set_vram_bank(0x04, p + num + 0x1000, true);
	nes::ppu::set_vram_bank(0x05, p + num + 0x1400, true);
	nes::ppu::set_vram_bank(0x06, p + num + 0x1800, true);
	nes::ppu::set_vram_bank(0x07, p + num + 0x1c00, true);
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
