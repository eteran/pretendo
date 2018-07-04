
#include "Mapper040.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(40)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper40::Mapper40() : irq_counter_(0), irq_enabled_(false) {

	set_prg_67(6);
	set_prg_89(4);
	set_prg_ab(5);
	set_prg_cd(0);
	set_prg_ef(7);

	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
std::string Mapper40::name() const {
	return "SMB2j Pirate";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper40::write_8(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	irq_enabled_ = false;
	irq_counter_ = 0;
	nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper40::write_9(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	irq_enabled_ = false;
	irq_counter_ = 0;
	nes::cpu::clear_irq(nes::cpu::MAPPER_IRQ);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper40::write_a(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	irq_enabled_ = true;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper40::write_b(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	irq_enabled_ = true;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper40::write_e(uint16_t address, uint8_t value) {
	(void)address;
	set_prg_cd(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper40::write_f(uint16_t address, uint8_t value) {
	(void)address;
	set_prg_cd(value & 0x07);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper40::cpu_sync() {
	if(irq_enabled_) {
		irq_counter_ = (irq_counter_ + 1) % 0x1000;
		if(irq_counter_ == 0) {
			nes::cpu::irq(nes::cpu::MAPPER_IRQ);
		}
	}
}
