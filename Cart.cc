
#include "Cart.h"
#include "Mapper.h"
#include "NES.h"
#include <cstring>
#include <iostream>
#include <iomanip>

namespace {

//------------------------------------------------------------------------------
// Name: create_mask
//------------------------------------------------------------------------------
size_t create_mask(size_t size) {

	// returns 1 less than closest fitting power of 2
	// is this number not a power of two or 0?
	if((size & (size - 1)) != 0) {
		// yea, fix it!
		--size;
		size |= size >> 1;
		size |= size >> 2;
		size |= size >> 4;
		size |= size >> 8;
		size |= size >> 16;
		++size;
	} else if(size == 0) {
		++size;
	}

	return --size;
}

//------------------------------------------------------------------------------
// Name: is_power_of_2
//------------------------------------------------------------------------------
bool is_power_of_2(size_t size) {
	return (size & (size - 1)) == 0;
}

}

//------------------------------------------------------------------------------
// Name: Cart
//------------------------------------------------------------------------------
Cart::Cart() : prg_mask_(0), chr_mask_(0), prg_hash_(0), chr_hash_(0), rom_hash_(0), mirroring_(MIR_HORIZONTAL){
	memset(&cart_, 0, sizeof(cart_));
}

//------------------------------------------------------------------------------
// Name: ~Cart
//------------------------------------------------------------------------------
Cart::~Cart() {
	unload();
}

//------------------------------------------------------------------------------
// Name: load
//------------------------------------------------------------------------------
void Cart::load(const std::string &s) {

	std::cout << "[Cart::load] loading '" << s << "'...";

	const INES_RETURN_CODE r = load_file_INES(s.c_str(), &cart_);
	if(r == INES_OK) {
		std::cout << " OK!" << std::endl;

		// get mask values
		prg_mask_ = create_mask(cart_.prg_size);
		chr_mask_ = create_mask(cart_.chr_size);

		switch(cart_.mirroring) {
		case MIRR_HORIZONTAL: mirroring_ = MIR_HORIZONTAL; break;
		case MIRR_VERTICAL:   mirroring_ = MIR_VERTICAL;   break;
		case MIRR_4SCREEN:    mirroring_ = MIR_4SCREEN;    break;
		default:              mirroring_ = MIR_MAPPER;     break;
		}

		prg_hash_ = prg_hash_INES(&cart_);
		chr_hash_ = chr_hash_INES(&cart_);
		rom_hash_ = rom_hash_INES(&cart_);

		std::cout << "PRG HASH: " << std::hex << std::setw(8) << std::setfill('0') << prg_hash_ << std::dec << std::endl;
		std::cout << "CHR HASH: " << std::hex << std::setw(8) << std::setfill('0') << chr_hash_ << std::dec << std::endl;
		std::cout << "ROM HASH: " << std::hex << std::setw(8) << std::setfill('0') << rom_hash_ << std::dec << std::endl;

		if(!is_power_of_2(cart_.prg_size)) {
			std::cout << "WARNING: PRG size is not a power of 2, this is unusual" << std::endl;
		}

		if(!is_power_of_2(cart_.chr_size)) {
			std::cout << "WARNING: CHR size is not a power of 2, this is unusual" << std::endl;
		}

		mapper_ = Mapper::create_mapper(cart_.mapper);
	} else {
		std::cout << " ERROR! (" << r << ")" << std::endl;
		mapper_   = boost::shared_ptr<Mapper>();
		prg_hash_ = 0;
		chr_hash_ = 0;
		rom_hash_ = 0;
	}
}

//------------------------------------------------------------------------------
// Name: unload
//------------------------------------------------------------------------------
void Cart::unload() {
	free_file_INES(&cart_);
	memset(&cart_, 0, sizeof(cart_));
	mapper_.reset();
}

//------------------------------------------------------------------------------
// Name: has_chr_rom
//------------------------------------------------------------------------------
bool Cart::has_chr_rom() const {
	return cart_.chr_rom != 0;
}

//------------------------------------------------------------------------------
// Name: prg_mask
//------------------------------------------------------------------------------
uint32_t Cart::prg_mask() const {
	return prg_mask_;
}

//------------------------------------------------------------------------------
// Name: chr_mask
//------------------------------------------------------------------------------
uint32_t Cart::chr_mask() const {
	return chr_mask_;
}

//------------------------------------------------------------------------------
// Name: prg
//------------------------------------------------------------------------------
uint8_t *Cart::prg() const {
	return cart_.prg_rom;
}

//------------------------------------------------------------------------------
// Name: chr
//------------------------------------------------------------------------------
uint8_t *Cart::chr() const {
	return cart_.chr_rom;
}

//------------------------------------------------------------------------------
// Name: mirroring
//------------------------------------------------------------------------------
Cart::MIRRORING Cart::mirroring() const {
	return mirroring_;
}

//------------------------------------------------------------------------------
// Name: mapper
//------------------------------------------------------------------------------
const boost::shared_ptr<Mapper> &Cart::mapper() const {
	return mapper_;
}

//------------------------------------------------------------------------------
// Name: prg_hash
//------------------------------------------------------------------------------
uint32_t Cart::prg_hash() const {
	return prg_hash_;
}

//------------------------------------------------------------------------------
// Name: chr_hash
//------------------------------------------------------------------------------
uint32_t Cart::chr_hash() const {
	return chr_hash_;
}

//------------------------------------------------------------------------------
// Name: rom_hash
//------------------------------------------------------------------------------
uint32_t Cart::rom_hash() const {
	return rom_hash_;
}

//------------------------------------------------------------------------------
// Name: raw_image
//------------------------------------------------------------------------------
std::vector<uint8_t> Cart::raw_image() const {

	const uint8_t *const prg_rom = prg();
	const uint8_t *const chr_rom = chr();

	// create a vector and copy the PRG into it
	std::vector<uint8_t> image(prg_rom, prg_rom + cart_.prg_size);

	// if there is CHR, insert it at the end of the vector
	if(chr_rom) {
		image.insert(image.end(), chr_rom, chr_rom + cart_.chr_size);
	}

 	return image;
}
