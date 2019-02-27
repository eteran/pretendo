
#ifndef CART_20120303_H_
#define CART_20120303_H_

#include "iNES/Rom.h"
#include <string>
#include <vector>
#include <memory>

class Mapper;

class Cart {
public:
    Cart()                        = default;
    Cart(const Cart &)            = delete;
    Cart &operator=(const Cart &) = delete;
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
    const std::string &filename() const;
	bool load(const std::string &s);
	void unload();
	bool has_chr_rom() const;
	uint32_t prg_mask() const;
	uint32_t chr_mask() const;
	uint32_t prg_hash() const;
	uint32_t chr_hash() const;
	uint32_t rom_hash() const;
	uint8_t *prg() const;
	uint8_t *chr() const;
	MIRRORING mirroring() const;
	const std::shared_ptr<Mapper> &mapper() const { return mapper_; }
	std::vector<uint8_t> raw_image() const;

private:    
	std::shared_ptr<iNES::Rom> rom_;
    uint32_t                   prg_mask_  = 0;
    uint32_t                   chr_mask_  = 0;
    uint32_t                   prg_hash_  = 0;
    uint32_t                   chr_hash_  = 0;
    uint32_t                   rom_hash_  = 0;
    MIRRORING                  mirroring_ = MIR_HORIZONTAL;
	std::shared_ptr<Mapper>    mapper_;
    std::string                filename_;
};

#endif

