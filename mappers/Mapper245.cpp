
#include "Mapper245.h"
#include "Cart.h"
#include "Nes.h"
#include "Ppu.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(245)

// TODO: implement this correctly...
//       this basically just copies the Mapper4 implementation

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper245::name() const {
	return "iNES 245 (MMC3 Clone)";
}
