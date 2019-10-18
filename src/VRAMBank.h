
#ifndef VRAM_BANK_20110314_H_
#define VRAM_BANK_20110314_H_

#include <cstdint>
#include <cstddef>

using std::uint8_t;

class VRAMBank {
public:
    VRAMBank()                               = default;
    VRAMBank(const VRAMBank &other)          = default;
    VRAMBank &operator=(const VRAMBank &rhs) = default;

	VRAMBank &operator=(std::nullptr_t) {
		ptr_ = nullptr;
		writeable_ = false;
		return *this;
	}

	explicit VRAMBank(std::nullptr_t) : ptr_(nullptr), writeable_(false) {
	}

	VRAMBank(uint8_t *p, bool writeable) : ptr_(p), writeable_(writeable) {
	}

public:
	uint8_t operator[](size_t n) const { return ptr_[n]; }
	uint8_t &operator[](size_t n)      { return ptr_[n]; }
	explicit operator bool() const     { return ptr_; }

public:
	bool writeable() const   { return writeable_; }

private:
    uint8_t* ptr_       = nullptr;
    bool     writeable_ = false;
};

#endif
