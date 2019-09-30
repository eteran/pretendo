
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
// Name: ~Mapper()
//------------------------------------------------------------------------------
Mapper::~Mapper() {
	// reset banks to not point anywhere
	nes::cpu::unmap_67();
	nes::cpu::unmap_89();
	nes::cpu::unmap_ab();
	nes::cpu::unmap_cd();
	nes::cpu::unmap_ef();

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
	nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_1
//------------------------------------------------------------------------------
void Mapper::write_1(uint16_t address, uint8_t value) {
	nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_2
//------------------------------------------------------------------------------
void Mapper::write_2(uint16_t address, uint8_t value) {

	switch(address & 0x07) {
	case 0x00:
		nes::ppu::write2000(value);
		break;
	case 0x01:
		nes::ppu::write2001(value);
		break;
	case 0x02:
		nes::ppu::write2002(value);
		break;
	case 0x03:
		nes::ppu::write2003(value);
		break;
	case 0x04:
		nes::ppu::write2004(value);
		break;
	case 0x05:
		nes::ppu::write2005(value);
		break;
	case 0x06:
		nes::ppu::write2006(value);
		break;
	case 0x07:
		nes::ppu::write2007(value);
		break;
	default:
		abort();
	}
}

//------------------------------------------------------------------------------
// Name: write_3
//------------------------------------------------------------------------------
void Mapper::write_3(uint16_t address, uint8_t value) {

	switch(address & 0x07) {
	case 0x00:
		nes::ppu::write2000(value);
		break;
	case 0x01:
		nes::ppu::write2001(value);
		break;
	case 0x02:
		nes::ppu::write2002(value);
		break;
	case 0x03:
		nes::ppu::write2003(value);
		break;
	case 0x04:
		nes::ppu::write2004(value);
		break;
	case 0x05:
		nes::ppu::write2005(value);
		break;
	case 0x06:
		nes::ppu::write2006(value);
		break;
	case 0x07:
		nes::ppu::write2007(value);
		break;
	default:
		abort();
	}
}

//------------------------------------------------------------------------------
// Name: Write4
//------------------------------------------------------------------------------
void Mapper::write_4(uint16_t address, uint8_t value) {

	switch(address) {
	case 0x4000:
		nes::apu::write4000(value);
		break;
	case 0x4001:
		nes::apu::write4001(value);
		break;
	case 0x4002:
		nes::apu::write4002(value);
		break;
	case 0x4003:
		nes::apu::write4003(value);
		break;
	case 0x4004:
		nes::apu::write4004(value);
		break;
	case 0x4005:
		nes::apu::write4005(value);
		break;
	case 0x4006:
		nes::apu::write4006(value);
		break;
	case 0x4007:
		nes::apu::write4007(value);
		break;
	case 0x4008:
		nes::apu::write4008(value);
		break;
	case 0x400a:
		nes::apu::write400A(value);
		break;
	case 0x400b:
		nes::apu::write400B(value);
		break;
	case 0x400c:
		nes::apu::write400C(value);
		break;
	case 0x400e:
		nes::apu::write400E(value);
		break;
	case 0x400f:
		nes::apu::write400F(value);
		break;
	case 0x4010:
		nes::apu::write4010(value);
		break;
	case 0x4011:
		nes::apu::write4011(value);
		break;
	case 0x4012:
		nes::apu::write4012(value);
		break;
	case 0x4013:
		nes::apu::write4013(value);
		break;
	case 0x4014:
		nes::ppu::write4014(value);
		break;
	case 0x4015:
		nes::apu::write4015(value);
		break;
	case 0x4016:
		nes::input::write4016(value);
		break;
	case 0x4017:
		nes::apu::write4017(value);
		break;
	}
}

//------------------------------------------------------------------------------
// Name: write_5
//------------------------------------------------------------------------------
void Mapper::write_5(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	// Dead end write, expansion port maybe?
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_6
//------------------------------------------------------------------------------
void Mapper::write_6(uint16_t address, uint8_t value) {
	nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_7
//------------------------------------------------------------------------------
void Mapper::write_7(uint16_t address, uint8_t value) {
	nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_8
//------------------------------------------------------------------------------
void Mapper::write_8(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_9
//------------------------------------------------------------------------------
void Mapper::write_9(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void Mapper::write_a(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Mapper::write_b(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Mapper::write_c(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Mapper::write_d(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Mapper::write_e(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void Mapper::write_f(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	//nes::cpu::write(address, value);
}

//------------------------------------------------------------------------------
// Name: read_0
//------------------------------------------------------------------------------
uint8_t Mapper::read_0(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_1
//------------------------------------------------------------------------------
uint8_t Mapper::read_1(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_2
//------------------------------------------------------------------------------
uint8_t Mapper::read_2(uint16_t address) {

	switch(address & 0x07) {
	case 0x02: return nes::ppu::read2002();
	case 0x04: return nes::ppu::read2004();
	case 0x07: return nes::ppu::read2007();
	default:
		return nes::ppu::read200x();
	}
}

//------------------------------------------------------------------------------
// Name: read_3
//------------------------------------------------------------------------------
uint8_t Mapper::read_3(uint16_t address) {
	switch(address & 0x07) {
	case 0x02: return nes::ppu::read2002();
	case 0x04: return nes::ppu::read2004();
	case 0x07: return nes::ppu::read2007();
	default:
		return nes::ppu::read200x();
	}
}

//------------------------------------------------------------------------------
// Name: read_4
//------------------------------------------------------------------------------
uint8_t Mapper::read_4(uint16_t address) {

	switch(address) {
	case 0x4015: return nes::apu::read4015();
	case 0x4016: return nes::input::read4016();
	case 0x4017: return nes::input::read4017();
	default:
		return static_cast<uint8_t>(address >> 8);
	}
}

//------------------------------------------------------------------------------
// Name: read_5
//------------------------------------------------------------------------------
uint8_t Mapper::read_5(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_6
//------------------------------------------------------------------------------
uint8_t Mapper::read_6(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_7
//------------------------------------------------------------------------------
uint8_t Mapper::read_7(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_8
//------------------------------------------------------------------------------
uint8_t Mapper::read_8(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_9
//------------------------------------------------------------------------------
uint8_t Mapper::read_9(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_a
//------------------------------------------------------------------------------
uint8_t Mapper::read_a(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_b
//------------------------------------------------------------------------------
uint8_t Mapper::read_b(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_c
//------------------------------------------------------------------------------
uint8_t Mapper::read_c(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_d
//------------------------------------------------------------------------------
uint8_t Mapper::read_d(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_e
//------------------------------------------------------------------------------
uint8_t Mapper::read_e(uint16_t address) {
	return nes::cpu::read(address);
}

//------------------------------------------------------------------------------
// Name: read_f
//------------------------------------------------------------------------------
uint8_t Mapper::read_f(uint16_t address) {
	return nes::cpu::read(address);
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
	nes::cpu::swap_67(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_89
//------------------------------------------------------------------------------
void Mapper::set_prg_89(int num) {
	num *= (8 * 1024);
	nes::cpu::swap_89(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_ab
//------------------------------------------------------------------------------
void Mapper::set_prg_ab(int num) {
	num *= (8 * 1024);
	nes::cpu::swap_ab(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_cd
//------------------------------------------------------------------------------
void Mapper::set_prg_cd(int num) {
	num *= (8 * 1024);
	nes::cpu::swap_cd(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_ef
//------------------------------------------------------------------------------
void Mapper::set_prg_ef(int num) {
	num *= (8 * 1024);
	nes::cpu::swap_ef(nes::cart.prg() + (num & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_89ab
//------------------------------------------------------------------------------
void Mapper::set_prg_89ab(int num) {
	num *= (16 * 1024);
	nes::cpu::swap_89(nes::cart.prg() + ((num + 0x0000) & nes::cart.prg_mask()));
	nes::cpu::swap_ab(nes::cart.prg() + ((num + 0x2000) & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_cdef
//------------------------------------------------------------------------------
void Mapper::set_prg_cdef(int num) {
	num *= (16 * 1024);
	nes::cpu::swap_cd(nes::cart.prg() + ((num + 0x0000) & nes::cart.prg_mask()));
	nes::cpu::swap_ef(nes::cart.prg() + ((num + 0x2000) & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_prg_89abcdef
//------------------------------------------------------------------------------
void Mapper::set_prg_89abcdef(int num) {
	num *= (32 * 1024);
	nes::cpu::swap_89(nes::cart.prg() + ((num + 0x0000) & nes::cart.prg_mask()));
	nes::cpu::swap_ab(nes::cart.prg() + ((num + 0x2000) & nes::cart.prg_mask()));
	nes::cpu::swap_cd(nes::cart.prg() + ((num + 0x4000) & nes::cart.prg_mask()));
	nes::cpu::swap_ef(nes::cart.prg() + ((num + 0x6000) & nes::cart.prg_mask()));
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_03ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_03ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0400_07ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0400_07ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0800_0bff
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0bff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0c00_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0c00_0fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_13ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_13ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1400_17ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1400_17ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1800_1bff
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1bff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1c00_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1c00_1fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (1 * 1024);
		set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_07ff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_07ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0800_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0800_0fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_17ff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_17ff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1800_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1800_1fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (2 * 1024);
		set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_0fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_0fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (4 * 1024);
		set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
		set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0800) & nes::cart.chr_mask()), false);
		set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0c00) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_1000_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_1000_1fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (4 * 1024);
		set_vram_bank(0x04, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		set_vram_bank(0x05, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
		set_vram_bank(0x06, nes::cart.chr() + ((num + 0x0800) & nes::cart.chr_mask()), false);
		set_vram_bank(0x07, nes::cart.chr() + ((num + 0x0c00) & nes::cart.chr_mask()), false);
	}
}

//------------------------------------------------------------------------------
// Name: set_chr_0000_1fff
//------------------------------------------------------------------------------
void Mapper::set_chr_0000_1fff(int num) {
	if(LIKELY(nes::cart.has_chr_rom())) {
		num *= (8 * 1024);
		set_vram_bank(0x00, nes::cart.chr() + ((num + 0x0000) & nes::cart.chr_mask()), false);
		set_vram_bank(0x01, nes::cart.chr() + ((num + 0x0400) & nes::cart.chr_mask()), false);
		set_vram_bank(0x02, nes::cart.chr() + ((num + 0x0800) & nes::cart.chr_mask()), false);
		set_vram_bank(0x03, nes::cart.chr() + ((num + 0x0c00) & nes::cart.chr_mask()), false);
		set_vram_bank(0x04, nes::cart.chr() + ((num + 0x1000) & nes::cart.chr_mask()), false);
		set_vram_bank(0x05, nes::cart.chr() + ((num + 0x1400) & nes::cart.chr_mask()), false);
		set_vram_bank(0x06, nes::cart.chr() + ((num + 0x1800) & nes::cart.chr_mask()), false);
		set_vram_bank(0x07, nes::cart.chr() + ((num + 0x1c00) & nes::cart.chr_mask()), false);
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
// Name: write_memory
//------------------------------------------------------------------------------
void Mapper::write_memory(uint16_t address, uint8_t value) {

	switch((address >> 12) & 0xf) {
	case 0x0000: write_0(address, value); break;
	case 0x0001: write_1(address, value); break;
	case 0x0002: write_2(address, value); break;
	case 0x0003: write_3(address, value); break;
	case 0x0004: write_4(address, value); break;
	case 0x0005: write_5(address, value); break;
	case 0x0006: write_6(address, value); break;
	case 0x0007: write_7(address, value); break;
	case 0x0008: write_8(address, value); break;
	case 0x0009: write_9(address, value); break;
	case 0x000a: write_a(address, value); break;
	case 0x000b: write_b(address, value); break;
	case 0x000c: write_c(address, value); break;
	case 0x000d: write_d(address, value); break;
	case 0x000e: write_e(address, value); break;
	case 0x000f: write_f(address, value); break;
	default:
		abort();
	}
}


//------------------------------------------------------------------------------
// Name: read_memory
//------------------------------------------------------------------------------
uint8_t Mapper::read_memory(uint16_t address) {
	switch((address >> 12) & 0xf) {
	case 0x0000: return read_0(address);
	case 0x0001: return read_1(address);
	case 0x0002: return read_2(address);
	case 0x0003: return read_3(address);
	case 0x0004: return read_4(address);
	case 0x0005: return read_5(address);
	case 0x0006: return read_6(address);
	case 0x0007: return read_7(address);
	case 0x0008: return read_8(address);
	case 0x0009: return read_9(address);
	case 0x000a: return read_a(address);
	case 0x000b: return read_b(address);
	case 0x000c: return read_c(address);
	case 0x000d: return read_d(address);
	case 0x000e: return read_e(address);
	case 0x000f: return read_f(address);
	default:
		abort();
	}
}

//------------------------------------------------------------------------------
// Name: set_mirroring
//------------------------------------------------------------------------------
void Mapper::set_mirroring(uint8_t mir) {
	nes::ppu::set_mirroring(mir);
}

//------------------------------------------------------------------------------
// Name: set_vram_bank
//------------------------------------------------------------------------------
void Mapper::set_vram_bank(uint8_t bank, uint8_t *p, bool writeable) {
	nes::ppu::set_vram_bank(bank, p, writeable);
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
