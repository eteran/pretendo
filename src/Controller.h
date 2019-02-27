
#ifndef CONTROLLER_20080314_H_
#define CONTROLLER_20080314_H_

#include "ShiftRegister.h"
#include <bitset>
#include <cstdint>

using std::uint8_t;

class Controller {
public:
    Controller()                              = default;
    Controller(const Controller &)            = delete;
	Controller &operator=(const Controller &) = delete;

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
    ShiftRegister<uint8_t> data_{ 0 };
    uint8_t                read_index_ = 0;
    bool                   connected_ = true;
};


#endif
