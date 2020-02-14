
#ifndef LFSR_20110314_H_
#define LFSR_20110314_H_

#include <cstdint>

namespace nes {
namespace apu {

using std::uint16_t;
using std::uint8_t;

class LFSR {
public:
	//-------------------------------------------------------------------
	// Name: set_mode
	//-------------------------------------------------------------------
	void set_mode(uint8_t value) {
		mode_ = value;
	}

	//-------------------------------------------------------------------
	// Name: load
	//-------------------------------------------------------------------
	void load(uint8_t value) {
		value_ = value;
	}

	//-------------------------------------------------------------------
	// Name: clock
	//-------------------------------------------------------------------
	void clock() {
		const uint16_t f = feedback();
		value_ >>= 1;
		value_ = (value_ & ~0x4000) | (f << 14);
	}

	//-------------------------------------------------------------------
	// Name: value
	//-------------------------------------------------------------------
	uint16_t value() const {
		return value_ & 0x7fff;
	}

private:
	//-------------------------------------------------------------------
	// Name: feedback
	//-------------------------------------------------------------------
	uint16_t feedback() const {
		if (mode_) {
			return (value_ & 0x01) ^ ((value_ >> 6) & 0x01);
		} else {
			return (value_ & 0x01) ^ ((value_ >> 1) & 0x01);
		}
	}

private:
	uint16_t value_ = 1;
	uint8_t mode_   = 0;
};

}
}

#endif
