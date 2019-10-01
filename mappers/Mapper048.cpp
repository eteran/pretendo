
#include "Mapper048.h"
#include "Ppu.h"
#include "Cart.h"
#include "Nes.h"
#include <cassert>

SETUP_STATIC_INES_MAPPER_REGISTRAR(48)

//#define MMC3A_STYLE

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper48::Mapper48() {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper48::name() const {
	return "iNES 48 (Taito)";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_8(uint16_t address, uint8_t value) {
	 write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_9(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_a(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_b(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_c(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_d(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_e(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_f(uint16_t address, uint8_t value) {
	write_handler(address, value);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::write_handler(uint16_t address, uint8_t value) {

	switch(address & 0xe003) {
	case 0x8000:
		set_prg_89(value);
		break;
	case 0x8001:
		set_prg_ab(value);
		break;
	case 0x8002:
		set_chr_0000_07ff(value);
		break;
	case 0x8003:
		set_chr_0800_0fff(value);
		break;
	case 0xa000:
		set_chr_1000_13ff(value);
		break;
	case 0xa001:
		set_chr_1400_17ff(value);
		break;
	case 0xa002:
		set_chr_1800_1bff(value);
		break;
	case 0xa003:
		set_chr_1c00_1fff(value);
		break;
	case 0xc000:
		irq_latch_ = value ^ 0xff;
		break;
	case 0xc001:
		irq_counter_ = 0x00;
		irq_reload_ = true;
		break;
	case 0xc002:
		irq_enabled_ = true;
		break;
	case 0xc003:
		irq_enabled_ = false;
		nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
		break;
	case 0xe000:
		if(!(value & 0x40)) {
			set_mirroring(nes::ppu::mirror_vertical);
		} else {
			set_mirroring(nes::ppu::mirror_horizontal);
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::vram_change_hook(uint16_t vram_address) {

	if(vram_address & 0x1000 && !(prev_vram_address_ & 0x1000)) {
		if ((nes::ppu::cycle_count() - prev_ppu_cycle_) >= 16) {
			clock_irq();
		}

		prev_ppu_cycle_ = nes::ppu::cycle_count();
	}

	prev_vram_address_ = vram_address;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper48::clock_irq() {

	/*
	 * When the counter is clocked, if it's not zero, it is decremented,
	 * otherwise it is reloaded with the last value written to $C000. *After*
	 * decrementing/reloading, if the counter is zero and IRQ is enabled via
	 * $E001, the IRQ flag is set.
	 */

	if(irq_counter_ == 0x00) {
		irq_counter_ = irq_latch_;
	} else {
		--irq_counter_;
		irq_reload_ = true;
	}
#ifdef MMC3A_STYLE
	if(irq_enabled_ && irq_counter_ == 0 && irq_reload_) {
#else
	if(irq_enabled_ && irq_counter_ == 0) {
#endif
		nes::cpu::irq(nes::cpu::MAPPER_IRQ);
	}

	irq_reload_ = false;
}
