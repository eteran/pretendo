
#include "VRC2.h"
#include "Cpu.h"

// TODO(eteran): implement sub-mapper madness...

//------------------------------------------------------------------------------
// Name: VRC2
//------------------------------------------------------------------------------
VRC2::VRC2(int chr_shift) : chr_shift_(chr_shift) {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	set_chr_0000_1fff(0);

	chr_[0] = 0;
	chr_[1] = 1;
	chr_[2] = 2;
	chr_[3] = 3;
	chr_[4] = 4;
	chr_[5] = 5;
	chr_[6] = 6;
	chr_[7] = 7;
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string VRC2::name() const {
	return "VRC2";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t VRC2::read_6(uint16_t address) {
	return (address >> 8) | latch_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC2::write_6(uint16_t address, uint8_t value) {
	(void)address;
	latch_ = (latch_ & 0xfe) | (value & 0x01);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC2::write_8(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		set_prg_89(value & 0x1f);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC2::write_9(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0x9000:
	case 0x9001:
	case 0x9002:
	case 0x9003:
		switch(value & 0x03) {
		case 0x00: set_mirroring(mirror_vertical);    break;
		case 0x01: set_mirroring(mirror_horizontal);  break;
		case 0x02: set_mirroring(mirror_single_low);  break;
		case 0x03: set_mirroring(mirror_single_high); break;
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC2::write_a(uint16_t address, uint8_t value) {
	(void)address;
	(void)value;
	switch(address & 0xf003) {
	case 0xa000:
	case 0xa001:
	case 0xa002:
	case 0xa003:
		set_prg_ab(value & 0x1f);
		break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC2::write_b(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xb000: chr_[0] = (chr_[0] & 0xf0) | ((value & 0x0f) << 0); set_chr_0000_03ff(chr_[0] >> chr_shift_); break;
	case 0xb001: chr_[0] = (chr_[0] & 0x0f) | ((value & 0x0f) << 4); set_chr_0000_03ff(chr_[0] >> chr_shift_); break;
	case 0xb002: chr_[1] = (chr_[1] & 0xf0) | ((value & 0x0f) << 0); set_chr_0400_07ff(chr_[1] >> chr_shift_); break;
	case 0xb003: chr_[1] = (chr_[1] & 0x0f) | ((value & 0x0f) << 4); set_chr_0400_07ff(chr_[1] >> chr_shift_); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC2::write_c(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xc000: chr_[2] = (chr_[2] & 0xf0) | ((value & 0x0f) << 0); set_chr_0800_0bff(chr_[2] >> chr_shift_); break;
	case 0xc001: chr_[2] = (chr_[2] & 0x0f) | ((value & 0x0f) << 4); set_chr_0800_0bff(chr_[2] >> chr_shift_); break;
	case 0xc002: chr_[3] = (chr_[3] & 0xf0) | ((value & 0x0f) << 0); set_chr_0c00_0fff(chr_[3] >> chr_shift_); break;
	case 0xc003: chr_[3] = (chr_[3] & 0x0f) | ((value & 0x0f) << 4); set_chr_0c00_0fff(chr_[3] >> chr_shift_); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC2::write_d(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xd000: chr_[4] = (chr_[4] & 0xf0) | ((value & 0x0f) << 0); set_chr_1000_13ff(chr_[4] >> chr_shift_); break;
	case 0xd001: chr_[4] = (chr_[4] & 0x0f) | ((value & 0x0f) << 4); set_chr_1000_13ff(chr_[4] >> chr_shift_); break;
	case 0xd002: chr_[5] = (chr_[5] & 0xf0) | ((value & 0x0f) << 0); set_chr_1400_17ff(chr_[5] >> chr_shift_); break;
	case 0xd003: chr_[5] = (chr_[5] & 0x0f) | ((value & 0x0f) << 4); set_chr_1400_17ff(chr_[5] >> chr_shift_); break;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void VRC2::write_e(uint16_t address, uint8_t value) {
	switch(address & 0xf003) {
	case 0xe000: chr_[6] = (chr_[6] & 0xf0) | ((value & 0x0f) << 0); set_chr_1800_1bff(chr_[6] >> chr_shift_); break;
	case 0xe001: chr_[6] = (chr_[6] & 0x0f) | ((value & 0x0f) << 4); set_chr_1800_1bff(chr_[6] >> chr_shift_); break;
	case 0xe002: chr_[7] = (chr_[7] & 0xf0) | ((value & 0x0f) << 0); set_chr_1c00_1fff(chr_[7] >> chr_shift_); break;
	case 0xe003: chr_[7] = (chr_[7] & 0x0f) | ((value & 0x0f) << 4); set_chr_1c00_1fff(chr_[7] >> chr_shift_); break;
	}
}

