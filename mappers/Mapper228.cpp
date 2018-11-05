
#include "Mapper228.h"
#include "PPU.h"
#include "NES.h"
#include "Cart.h"
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(228)

namespace {
const uint8_t *prg_chips[4];
constexpr uint32_t ChipSize = 0x080000;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper228::Mapper228() {

	ram_[0] = 0;
	ram_[1] = 0;
	ram_[2] = 0;
	ram_[3] = 0;

	rom_[0] = nullptr;
	rom_[1] = nullptr;
	rom_[2] = nullptr;
	rom_[3] = nullptr;
	rom_[4] = nullptr;
	rom_[5] = nullptr;
	rom_[6] = nullptr;
	rom_[7] = nullptr;

	prg_chips[0] = nes::cart.prg() + ChipSize * 0;
	prg_chips[1] = nes::cart.prg() + ChipSize * 1;
	prg_chips[2] = nullptr;
	prg_chips[3] = nes::cart.prg() + ChipSize * 2;

	write_hander(0x8000, 0x00);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper228::name() const {
	return "Action 52";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_4(uint16_t address) {
	if(address >= 0x4020) {
		return ram_[address & 0x03];
	}
	return Mapper::read_4(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_5(uint16_t address) {
	return ram_[address & 0x03];
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_4(uint16_t address, uint8_t value) {

	if(address >= 0x4020) {
		ram_[address & 0x03] = value & 0x03;
	} else {
		Mapper::write_4(address, value);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_5(uint16_t address, uint8_t value) {
	ram_[address & 0x03] = value & 0x03;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_8(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_9(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_a(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_b(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_c(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_d(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_e(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_f(uint16_t address, uint8_t value) {
	write_hander(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper228::write_hander(uint16_t address, uint8_t value) {

	if(address & 0x2000) {
		nes::ppu::set_mirroring(nes::ppu::mirror_horizontal);
	} else {
		nes::ppu::set_mirroring(nes::ppu::mirror_vertical);
	}

	uint8_t prg_chip_select        = (address >> 11) & 0x03;
	const uint8_t  prg_page_select = (address >> 6) & 0x1f;
	const uint8_t  prg_mode        = ((address & 0x20) != 0);
	const uint8_t  chr_select      = ((address & 0x0f) << 2) | (value & 0x03);
	const uint32_t prg_mask        = nes::cart.prg_mask();

	set_chr_0000_1fff(chr_select);

	assert(prg_chip_select != 2);

	// Cheetahmen II seems to only have one chip
	if(prg_mask < (ChipSize - 1)) {
		prg_chip_select = 0;
	}

	if(prg_chips[prg_chip_select]) {
		if(prg_mode == 0) {

			const uint8_t *const ptr = prg_chips[prg_chip_select] + (((prg_page_select >> 1) * 0x8000) & (ChipSize - 1));

			rom_[0] = ptr + 0x0000;
			rom_[1] = ptr + 0x1000;
			rom_[2] = ptr + 0x2000;
			rom_[3] = ptr + 0x3000;
			rom_[4] = ptr + 0x4000;
			rom_[5] = ptr + 0x5000;
			rom_[6] = ptr + 0x6000;
			rom_[7] = ptr + 0x7000;

		} else if(prg_mode == 1) {

			const uint8_t *const ptr = prg_chips[prg_chip_select] + ((prg_page_select * 0x4000) & (ChipSize - 1));

			rom_[0] = ptr + 0x0000;
			rom_[1] = ptr + 0x1000;
			rom_[2] = ptr + 0x2000;
			rom_[3] = ptr + 0x3000;
			rom_[4] = ptr + 0x0000;
			rom_[5] = ptr + 0x1000;
			rom_[6] = ptr + 0x2000;
			rom_[7] = ptr + 0x3000;
		}
	} else {
		rom_[0] = nullptr;
		rom_[1] = nullptr;
		rom_[2] = nullptr;
		rom_[3] = nullptr;
		rom_[4] = nullptr;
		rom_[5] = nullptr;
		rom_[6] = nullptr;
		rom_[7] = nullptr;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_8(uint16_t address) {
	//return Mapper::read_8(address);

	if(rom_[0]) {
		return rom_[0][address & 0x0fff];
	}

	return Mapper::read_8(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_9(uint16_t address) {
	if(rom_[1]) {
		return rom_[1][address & 0x0fff];
	}

	return Mapper::read_9(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_a(uint16_t address) {
	if(rom_[2]) {
		return rom_[2][address & 0x0fff];
	}

	return Mapper::read_a(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_b(uint16_t address) {
	if(rom_[3]) {
		return rom_[3][address & 0x0fff];
	}

	return Mapper::read_b(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_c(uint16_t address) {
	if(rom_[4]) {
		return rom_[4][address & 0x0fff];
	}

	return Mapper::read_c(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_d(uint16_t address) {
	if(rom_[5]) {
		return rom_[5][address & 0x0fff];
	}

	return Mapper::read_d(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_e(uint16_t address) {
	if(rom_[6]) {
		return rom_[6][address & 0x0fff];
	}

	return Mapper::read_e(address);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper228::read_f(uint16_t address) {
	if(rom_[7]) {
		return rom_[7][address & 0x0fff];
	}

	return Mapper::read_f(address);
}

