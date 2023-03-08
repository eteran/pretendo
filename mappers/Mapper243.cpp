
#include "Mapper243.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(243)

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Mapper243::Mapper243() {
	regs_[0] = 0;
	regs_[1] = 0;
	regs_[2] = 0;
	regs_[3] = 0;
	regs_[4] = 0;
	regs_[5] = 0;
	regs_[6] = 0;
	regs_[7] = 0;

	set_prg_89ab(0);
	set_prg_cdef(-1);
	set_chr_0000_1fff(0);
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper243::name() const {
	return "Sachen";
}

//------------------------------------------------------------------------------
// Name: write_4
//------------------------------------------------------------------------------
void Mapper243::write_4(uint32_t address, uint8_t value) {
	if (address >= 0x4020) {

		switch (address & 0x4101) {
		case 0x4100:
			address_ = value & 0x07;
			break;
		case 0x4101:
			regs_[address_] = value;

			const uint8_t prg = regs_[0x05] & 0x07;
			const uint8_t chr = ((regs_[0x02] & 0x01) << 3) | ((regs_[0x06] & 0x03) << 1) | (regs_[0x04] & 0x01);
			const uint8_t mir = regs_[0x07] >> 1;

			set_prg_89abcdef(prg);
			set_chr_0000_1fff(chr);

			switch (mir) {
			case 0x00:
				set_mirroring(mirror_horizontal);
				break;
			case 0x01:
				set_mirroring(mirror_vertical);
				break;
			case 0x02:
				set_mirroring(0x15);
				break;
			case 0x03:
				set_mirroring(mirror_single_high);
				break;
			}
			break;
		}
	}
}
