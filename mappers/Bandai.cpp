
#include "Bandai.h"

//------------------------------------------------------------------------------
// Name: Bandai
//------------------------------------------------------------------------------
Bandai::Bandai() {
	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff_ram(chr_ram_, 0);
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Bandai::name() const {
	return "Bandai FCG";
}

//------------------------------------------------------------------------------
// Name: read_eeprom
//------------------------------------------------------------------------------
uint8_t Bandai::read_eeprom(uint16_t address) {
	(void)address;
	return 0x00;
}

//------------------------------------------------------------------------------
// Name: read_6
//------------------------------------------------------------------------------
uint8_t Bandai::read_6(uint16_t address) {
	return read_eeprom(address);
}

//------------------------------------------------------------------------------
// Name: read_7
//------------------------------------------------------------------------------
uint8_t Bandai::read_7(uint16_t address) {
	return read_eeprom(address);
}

//------------------------------------------------------------------------------
// Name: write_6
//------------------------------------------------------------------------------
void Bandai::write_6(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_7
//------------------------------------------------------------------------------
void Bandai::write_7(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_8
//------------------------------------------------------------------------------
void Bandai::write_8(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_9
//------------------------------------------------------------------------------
void Bandai::write_9(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_a
//------------------------------------------------------------------------------
void Bandai::write_a(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_b
//------------------------------------------------------------------------------
void Bandai::write_b(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_c
//------------------------------------------------------------------------------
void Bandai::write_c(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_d
//------------------------------------------------------------------------------
void Bandai::write_d(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_e
//------------------------------------------------------------------------------
void Bandai::write_e(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_f
//------------------------------------------------------------------------------
void Bandai::write_f(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name: write_handler
//------------------------------------------------------------------------------
void Bandai::write_handler(uint16_t address, uint8_t value) {

	switch(address & 0x000f) {
	case 0x00: set_chr_0000_03ff(value); break;
	case 0x01: set_chr_0400_07ff(value); break;
	case 0x02: set_chr_0800_0bff(value); break;
	case 0x03: set_chr_0c00_0fff(value); break;
	case 0x04: set_chr_1000_13ff(value); break;
	case 0x05: set_chr_1400_17ff(value); break;
	case 0x06: set_chr_1800_1bff(value); break;
	case 0x07: set_chr_1c00_1fff(value); break;
	case 0x08:
		set_prg_89ab(value);
		break;
	case 0x09:
		switch(value & 0x03) {
		case 0: set_mirroring(mirror_vertical);    break;
		case 1: set_mirroring(mirror_horizontal);  break;
		case 2: set_mirroring(mirror_single_low);  break;
		case 3: set_mirroring(mirror_single_high); break;
		}
		break;
	case 0x0a:
		irq_enabled_ = (value & 0x01);
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		break;
	case 0x0b:
		irq_counter_.lo = value;
		break;
	case 0x0c:
		irq_counter_.hi = value;
		break;
	case 0x0d:
		// TODO: implement the EEPROM stuff
		break;
	}
}

//------------------------------------------------------------------------------
// Name: cpu_sync
//------------------------------------------------------------------------------
void Bandai::cpu_sync() {
	if(irq_enabled_) {
		if(--irq_counter_.raw == 0x0000) {
			nes::cpu::irq(nes::cpu::MAPPER_IRQ);
		}
	}
}
