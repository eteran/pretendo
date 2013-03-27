
#ifndef CONTROLLER_20080314_H_
#define CONTROLLER_20080314_H_

#include "ShiftRegister.h"
#include <boost/cstdint.hpp>
#include <bitset>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Controller {
public:
	Controller();

public:
	uint8_t read();
	void poll();

public:
	enum {
		INDEX_A      = 0x00,
		INDEX_B      = 0x01,
		INDEX_SELECT = 0x02,
		INDEX_START  = 0x03,
		INDEX_UP     = 0x04,
		INDEX_DOWN   = 0x05,
		INDEX_LEFT   = 0x06,
		INDEX_RIGHT  = 0x07
	};

public:
	std::bitset<8>  keystate_;

private:
	ShiftRegister<uint8_t> data_;
	bool                   enabled_;
};

#endif
