
#ifndef VRAM_BANK_20110314_H_
#define VRAM_BANK_20110314_H_

#include <boost/cstdint.hpp>

using boost::uint8_t;

class VRAMBank {
public:
	VRAMBank() : ptr_(0), writeable_(false) {
	}

	VRAMBank(uint8_t *p, bool writeable) : ptr_(p), writeable_(writeable) {
	}

	VRAMBank(const VRAMBank &other) : ptr_(other.ptr_), writeable_(other.writeable_) {
	}

	VRAMBank &operator=(const VRAMBank &rhs) {
		ptr_       = rhs.ptr_;
		writeable_ = rhs.writeable_;
		return *this;
	}

	uint8_t operator[](size_t n) const { return ptr_[n]; }
	uint8_t &operator[](size_t n)      { return ptr_[n]; }
	operator void*() const             { return reinterpret_cast<void *>(ptr_ != 0); }

public:
	bool writeable() const   { return writeable_; }

public:
	void set_pointer(uint8_t *p, bool writeable) {
		ptr_       = p;
		writeable_ = writeable;
	}

private:
	uint8_t* ptr_;
	bool     writeable_;
};

#endif
