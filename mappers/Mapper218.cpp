
#include "Mapper218.h"

SETUP_STATIC_INES_MAPPER_REGISTRAR(218)

//------------------------------------------------------------------------------
// Name: Mapper218
//------------------------------------------------------------------------------
Mapper218::Mapper218() {

	set_prg_89ab(0);
	set_prg_cdef(-1);
}

//------------------------------------------------------------------------------
// Name: name
//------------------------------------------------------------------------------
std::string Mapper218::name() const {
	return "iNES 218";
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t Mapper218::read_vram(uint32_t address) {

	// TODO: deal with mirroring somehow
	// it's in the iNES header

	switch ((address >> 10) & 0x0f) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		return Mapper::read_vram(address | 0x2000);
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0e:
	case 0x0f:
	default:
		return Mapper::read_vram(address);
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Mapper218::write_vram(uint32_t address, uint8_t value) {
	switch ((address >> 10) & 0x0f) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		Mapper::write_vram(address | 0x2000, value);
		break;
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0e:
	case 0x0f:
	default:
		Mapper::write_vram(address, value);
		break;
	}
}
