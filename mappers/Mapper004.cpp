
#include "Mapper004.h"
#include "Cart.h"
#include "Nes.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(4)

//------------------------------------------------------------------------------
// Name: Mapper4
//------------------------------------------------------------------------------
Mapper4::Mapper4() {

	switch (nes::cart.rom_hash()) {
	case 0xf312d1de:
	case 0xa512bdf6:
	case 0x633afe6f:
	case 0x1335cb05:
		mode_ = ModeA;
		break;
	default:
		mode_ = ModeB;
		break;
	}
}
