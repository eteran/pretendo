
#include "Bus.h"
#include "Apu.h"
#include "Cart.h"
#include "Cpu.h"
#include "Input.h"
#include "Mapper.h"
#include "Nes.h"
#include "Ppu.h"
#include <algorithm>
#include <cstdlib>

namespace nes {
namespace bus {
namespace {

uint8_t ram_[0x800];

//------------------------------------------------------------------------------
// Name: write_0
//------------------------------------------------------------------------------
void write_0(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_0(address, value);
	ram_[address & 0x7ff] = value;
}

//------------------------------------------------------------------------------
// Name: write_1
//------------------------------------------------------------------------------
void write_1(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_1(address, value);
	ram_[address & 0x7ff] = value;
}

//------------------------------------------------------------------------------
// Name: write_2
//------------------------------------------------------------------------------
void write_2(uint16_t address, uint8_t value) {

	nes::cart.mapper()->write_2(address, value);
	switch (address & 0x07) {
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
void write_3(uint16_t address, uint8_t value) {

	nes::cart.mapper()->write_3(address, value);
	switch (address & 0x07) {
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
void write_4(uint16_t address, uint8_t value) {

	nes::cart.mapper()->write_4(address, value);
	switch (address) {
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
void write_5(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_5(address, value);
}

//------------------------------------------------------------------------------
// Name: write_6
//------------------------------------------------------------------------------
void write_6(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_6(address, value);
}

//------------------------------------------------------------------------------
// Name: write_7
//------------------------------------------------------------------------------
void write_7(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_7(address, value);
}

//------------------------------------------------------------------------------
// Name: write_8
//------------------------------------------------------------------------------
void write_8(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_8(address, value);
}

//------------------------------------------------------------------------------
// Name: write_9
//------------------------------------------------------------------------------
void write_9(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_9(address, value);
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void write_a(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_a(address, value);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void write_b(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_b(address, value);
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void write_c(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_c(address, value);
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void write_d(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_d(address, value);
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void write_e(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_e(address, value);
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void write_f(uint16_t address, uint8_t value) {
	nes::cart.mapper()->write_f(address, value);
}

//------------------------------------------------------------------------------
// Name: read_0
//------------------------------------------------------------------------------
uint8_t read_0(uint16_t address) {
	return ram_[address & 0x7ff];
}

//------------------------------------------------------------------------------
// Name: read_1
//------------------------------------------------------------------------------
uint8_t read_1(uint16_t address) {
	return ram_[address & 0x7ff];
}

//------------------------------------------------------------------------------
// Name: read_2
//------------------------------------------------------------------------------
uint8_t read_2(uint16_t address) {

	switch (address & 0x07) {
	case 0x02:
		return nes::ppu::read2002();
	case 0x04:
		return nes::ppu::read2004();
	case 0x07:
		return nes::ppu::read2007();
	default:
		return nes::ppu::read200x();
	}
}

//------------------------------------------------------------------------------
// Name: read_3
//------------------------------------------------------------------------------
uint8_t read_3(uint16_t address) {
	switch (address & 0x07) {
	case 0x02:
		return nes::ppu::read2002();
	case 0x04:
		return nes::ppu::read2004();
	case 0x07:
		return nes::ppu::read2007();
	default:
		return nes::ppu::read200x();
	}
}

//------------------------------------------------------------------------------
// Name: read_4
//------------------------------------------------------------------------------
uint8_t read_4(uint16_t address) {

	switch (address) {
	case 0x4015:
		return nes::apu::read4015();
	case 0x4016:
		return nes::input::read4016();
	case 0x4017:
		return nes::input::read4017();
	default:
		return nes::cart.mapper()->read_4(address);
	}
}

//------------------------------------------------------------------------------
// Name: read_5
//------------------------------------------------------------------------------
uint8_t read_5(uint16_t address) {
	return nes::cart.mapper()->read_5(address);
}

//------------------------------------------------------------------------------
// Name: read_6
//------------------------------------------------------------------------------
uint8_t read_6(uint16_t address) {
	return nes::cart.mapper()->read_6(address);
}

//------------------------------------------------------------------------------
// Name: read_7
//------------------------------------------------------------------------------
uint8_t read_7(uint16_t address) {
	return nes::cart.mapper()->read_7(address);
}

//------------------------------------------------------------------------------
// Name: read_8
//------------------------------------------------------------------------------
uint8_t read_8(uint16_t address) {
	return nes::cart.mapper()->read_8(address);
}

//------------------------------------------------------------------------------
// Name: read_9
//------------------------------------------------------------------------------
uint8_t read_9(uint16_t address) {
	return nes::cart.mapper()->read_9(address);
}

//------------------------------------------------------------------------------
// Name: read_a
//------------------------------------------------------------------------------
uint8_t read_a(uint16_t address) {
	return nes::cart.mapper()->read_a(address);
}

//------------------------------------------------------------------------------
// Name: read_b
//------------------------------------------------------------------------------
uint8_t read_b(uint16_t address) {
	return nes::cart.mapper()->read_b(address);
}

//------------------------------------------------------------------------------
// Name: read_c
//------------------------------------------------------------------------------
uint8_t read_c(uint16_t address) {
	return nes::cart.mapper()->read_c(address);
}

//------------------------------------------------------------------------------
// Name: read_d
//------------------------------------------------------------------------------
uint8_t read_d(uint16_t address) {
	return nes::cart.mapper()->read_d(address);
}

//------------------------------------------------------------------------------
// Name: read_e
//------------------------------------------------------------------------------
uint8_t read_e(uint16_t address) {
	return nes::cart.mapper()->read_e(address);
}

//------------------------------------------------------------------------------
// Name: read_f
//------------------------------------------------------------------------------
uint8_t read_f(uint16_t address) {
	return nes::cart.mapper()->read_f(address);
}

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void write_memory(uint16_t address, uint8_t value) {
	switch ((address >> 12) & 0xf) {
	case 0x0000:
		write_0(address, value);
		break;
	case 0x0001:
		write_1(address, value);
		break;
	case 0x0002:
		write_2(address, value);
		break;
	case 0x0003:
		write_3(address, value);
		break;
	case 0x0004:
		write_4(address, value);
		break;
	case 0x0005:
		write_5(address, value);
		break;
	case 0x0006:
		write_6(address, value);
		break;
	case 0x0007:
		write_7(address, value);
		break;
	case 0x0008:
		write_8(address, value);
		break;
	case 0x0009:
		write_9(address, value);
		break;
	case 0x000a:
		write_a(address, value);
		break;
	case 0x000b:
		write_b(address, value);
		break;
	case 0x000c:
		write_c(address, value);
		break;
	case 0x000d:
		write_d(address, value);
		break;
	case 0x000e:
		write_e(address, value);
		break;
	case 0x000f:
		write_f(address, value);
		break;
	default:
		abort();
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t read_memory(uint16_t address) {
	switch ((address >> 12) & 0xf) {
	case 0x0000:
		return read_0(address);
	case 0x0001:
		return read_1(address);
	case 0x0002:
		return read_2(address);
	case 0x0003:
		return read_3(address);
	case 0x0004:
		return read_4(address);
	case 0x0005:
		return read_5(address);
	case 0x0006:
		return read_6(address);
	case 0x0007:
		return read_7(address);
	case 0x0008:
		return read_8(address);
	case 0x0009:
		return read_9(address);
	case 0x000a:
		return read_a(address);
	case 0x000b:
		return read_b(address);
	case 0x000c:
		return read_c(address);
	case 0x000d:
		return read_d(address);
	case 0x000e:
		return read_e(address);
	case 0x000f:
		return read_f(address);
	default:
		abort();
	}
}

//-------------------------------------------------------------------
// Name: trash_ram
//-------------------------------------------------------------------
void trash_ram() {
	// NOTE(eteran): this could be "random" bytes, but all zeros
	// is just an good as any other patterns
	std::fill_n(ram_, sizeof(ram_), 0);
}

}
}
