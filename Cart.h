
#ifndef CART_20120303_H_
#define CART_20120303_H_

#include "libunif/load_ines.h"
#include <string>
#include <boost/shared_ptr.hpp>

template <int N>
struct K {
	static const int value = N * 1024;
};

class Mapper;

class Cart {
public:
	Cart();
	~Cart();

public:
	enum MIRRORING {
		MIR_VERTICAL,
		MIR_HORIZONTAL,
		MIR_SINGLE_HIGH,
		MIR_SINGLE_LOW,
		MIR_4SCREEN,
		MIR_MAPPER
	};

public:
	void load(const std::string &s);
	void unload();
	int prg_pages() const;
	int chr_pages() const;
	bool has_chr_rom() const;
	uint32_t prg_mask() const;
	uint32_t chr_mask() const;
	uint32_t prg_hash() const;
	uint32_t chr_hash() const;
	uint32_t rom_hash() const;
	uint8_t *prg() const;
	uint8_t *chr() const;
	MIRRORING mirroring() const;
	const boost::shared_ptr<Mapper> &mapper() const;
	uint8_t *raw_image() const;

private:
	ines_cart_t               cart_;
	uint32_t                  prg_mask_;
	uint32_t                  chr_mask_;
	uint32_t                  prg_hash_;
	uint32_t                  chr_hash_;
	uint32_t                  rom_hash_;
	MIRRORING                 mirroring_;
	boost::shared_ptr<Mapper> mapper_;
};

#endif

