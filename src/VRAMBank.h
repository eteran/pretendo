
#ifndef VRAM_BANK_20110314_H_
#define VRAM_BANK_20110314_H_

#include <cstddef>
#include <cstdint>

class VRAMBank {
public:
	enum Type {
		Ram,
		Rom
	};

public:
	VRAMBank()                      = default;
	VRAMBank(const VRAMBank &other) = default;
	VRAMBank &operator=(const VRAMBank &rhs) = default;

	VRAMBank &operator=(std::nullptr_t) {
		ptr_  = nullptr;
		type_ = Rom;
		return *this;
	}

	explicit VRAMBank(std::nullptr_t)
		: ptr_(nullptr), type_(Rom) {
	}

	VRAMBank(uint8_t *p, Type type)
		: ptr_(p), type_(type) {
	}

public:
	uint8_t operator[](size_t n) const { return ptr_[n]; }
	uint8_t &operator[](size_t n) { return ptr_[n]; }
	explicit operator bool() const { return ptr_; }

public:
	bool writeable() const { return type_ == Ram; }

private:
	uint8_t *ptr_ = nullptr;
	Type type_    = Ram;
};

#endif
