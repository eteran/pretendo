
#include "Cart.h"
#include "Mapper.h"
#include <cstring>
#include <iostream>
#include <iomanip>

namespace {

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
size_t create_mask(size_t size) {
	// returns 1 less than closest fitting power of 2
	// is this number not a power of two or 0?
	if((size & (size - 1)) != 0) {
		//yea, fix it!
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

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Cart::Cart() : prg_mask_(0), chr_mask_(0), prg_hash_(0), chr_hash_(0), rom_hash_(0), mirroring_(MIR_HORIZONTAL){
	memset(&cart_, 0, sizeof(cart_));
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Cart::~Cart() {
	unload();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Cart::load(const std::string &s) {

	std::cout << "[Cart::load] loading '" << s << "'...";

	const UNIF_RETURN_CODE r = load_file_INES(s.c_str(), &cart_);
	if(r == UNIF_OK) {
		std::cout << " OK!" << std::endl;
		// get mask values
		prg_mask_ = create_mask(prg_pages() * K<16>::value);
		chr_mask_ = create_mask(chr_pages() * K<8>::value);

		switch(mirroring_INES(&cart_)) {
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

		mapper_ = Mapper::create_mapper(mapper_INES(&cart_));
	} else {
		std::cout << " ERROR! (" << r << ")" << std::endl;
		mapper_ = boost::shared_ptr<Mapper>();
		prg_hash_ = 0;
		chr_hash_ = 0;
		rom_hash_ = 0;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Cart::unload() {
	free_file_INES(&cart_);
	memset(&cart_, 0, sizeof(cart_));
	mapper_.reset();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
int Cart::prg_pages() const {
	return prg_size_INES(&cart_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
int Cart::chr_pages() const{
	return chr_size_INES(&cart_);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
bool Cart::has_chr_rom() const {
	return chr_pages() != 0;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint32_t Cart::prg_mask() const {
	return prg_mask_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint32_t Cart::chr_mask() const {
	return chr_mask_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t *Cart::prg() const {
	return cart_.prg_rom;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t *Cart::chr() const {
	return cart_.chr_rom;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
Cart::MIRRORING Cart::mirroring() const {
	return mirroring_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
const boost::shared_ptr<Mapper> &Cart::mapper() const {
	return mapper_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint32_t Cart::prg_hash() const {
	return prg_hash_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint32_t Cart::chr_hash() const {
	return chr_hash_;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint32_t Cart::rom_hash() const {
	return rom_hash_;
}
