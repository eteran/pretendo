
#ifndef CONTROLLER_20080314_H_
#define CONTROLLER_20080314_H_

#include "ShiftRegister.h"
#include <atomic>
#include <cstdint>

class Controller {
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
	void set_button(int index, bool pressed);
	void clear_buttons();

private:
	// written by the UI thread and read by the emulation thread, so it is a
	// single atomic word rather than a bitset: poll() latches the whole pad at
	// once, and per-button atomics would still allow a torn snapshot
	std::atomic<uint8_t> keystate_{0};
	ShiftRegister<uint8_t> data_{0};
	uint8_t read_index_ = 0;
	bool connected_     = true;
};

#endif
