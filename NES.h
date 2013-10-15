
#ifndef NES_H_
#define NES_H_

#include "Reset.h"
#include "APU.h"
#include "CPU.h"
#include "PPU.h"
#include "Cart.h"
#include "Input.h"
#include "Config.h"

class VideoInterface;

namespace nes {

	extern Input  input;
	extern CPU    cpu;
	extern PPU    ppu;
	extern APU    apu;
	extern Cart   cart;
	extern Config config;

	void run_frame(VideoInterface *video);
	void reset(RESET reset_type);
}

#endif
