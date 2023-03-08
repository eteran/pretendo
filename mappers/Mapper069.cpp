
#include "Mapper069.h"
#include "Cart.h"
#include "Nes.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(69)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper69::Mapper69() {

	prg_ptr_ = open_sram(0x2000);

	set_prg_67(0);
	set_prg_89ab(0);
	set_prg_cdef(-1);

	if (nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper69::name() const {
	return "Sunsoft 5B/FME-7";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper69::read_6(uint32_t address) {
	switch (prg_mode_ & 0xc0) {
	case 0x00:
		return Mapper::read_6(address);
	case 0xc0:
		return prg_ptr_[address & 0x1fff];
	}

	return (address >> 8);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper69::read_7(uint32_t address) {
	switch (prg_mode_ & 0xc0) {
	case 0x00:
		return Mapper::read_7(address);
	case 0xc0:
		return prg_ptr_[address & 0x1fff];
	}

	return (address >> 8);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper69::write_6(uint32_t address, uint8_t value) {
	switch (prg_mode_ & 0xc0) {
	case 0x00:
		break;
	case 0x80:
		break;
	case 0xc0:
		prg_ptr_[address & 0x1fff] = value;
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper69::write_7(uint32_t address, uint8_t value) {
	switch (prg_mode_ & 0xc0) {
	case 0x00:
		break;
	case 0x80:
		break;
	case 0xc0:
		prg_ptr_[address & 0x1fff] = value;
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper69::write_8(uint32_t address, uint8_t value) {
	(void)address;
	command_8000_ = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper69::write_a(uint32_t address, uint8_t value) {
	(void)address;

	switch (command_8000_ & 0x0f) {
	case 0x00:
		set_chr_0000_03ff(value);
		break;
	case 0x01:
		set_chr_0400_07ff(value);
		break;
	case 0x02:
		set_chr_0800_0bff(value);
		break;
	case 0x03:
		set_chr_0c00_0fff(value);
		break;
	case 0x04:
		set_chr_1000_13ff(value);
		break;
	case 0x05:
		set_chr_1400_17ff(value);
		break;
	case 0x06:
		set_chr_1800_1bff(value);
		break;
	case 0x07:
		set_chr_1c00_1fff(value);
		break;
	case 0x08:
		prg_mode_ = value;
		set_prg_67(value & 0x3F);
		break;
	case 0x09:
		set_prg_89(value);
		break;
	case 0x0a:
		set_prg_ab(value);
		break;
	case 0x0b:
		set_prg_cd(value);
		break;
	case 0x0c:
		switch (value & 0x3) {
		case 0:
			set_mirroring(mirror_vertical);
			break;
		case 1:
			set_mirroring(mirror_horizontal);
			break;
		case 2:
			set_mirroring(mirror_single_low);
			break;
		case 3:
			set_mirroring(mirror_single_high);
			break;
		}
		break;

	case 0x0d:
		irq_control_.raw = value;
		if (!irq_control_.enabled) {
			nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		}
		break;

	case 0x0e:
		irq_counter_.lo = value;
		break;

	case 0x0f:
		irq_counter_.hi = value;
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper69::write_c(uint32_t address, uint8_t value) {
	(void)address;
	command_c000_ = value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper69::write_e(uint32_t address, uint8_t value) {
	(void)address;
	(void)value;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper69::cpu_sync() {
	if (irq_control_.counter_enabled) {
		--irq_counter_.raw;
		if (irq_counter_.raw == 0xffff && irq_control_.enabled) {
			nes::cpu::irq(nes::cpu::MAPPER_IRQ);
		}
	}
}
