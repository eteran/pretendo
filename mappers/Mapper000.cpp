
#include "Mapper000.h"
#include "Cart.h"
#include "NES.h"
#include <cstring>

SETUP_STATIC_INES_MAPPER_REGISTRAR(0)

//------------------------------------------------------------------------------
// Name: Mapper0
//------------------------------------------------------------------------------
Mapper0::Mapper0() {

	set_prg_89ab(0);
	set_prg_cdef(-1);

	memset(chr_ram_, 0, sizeof(chr_ram_));

	if(nes::cart.has_chr_rom()) {
		set_chr_0000_1fff(0);
	} else {
		set_chr_0000_1fff_ram(chr_ram_, 0);
	}
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper0::name() const {
	return "NROM";
}
