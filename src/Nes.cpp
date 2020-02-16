
#include "Nes.h"
#include "Apu.h"
#include "Cart.h"
#include "Cpu.h"
#include "Input.h"
#include "Ppu.h"
#include <iostream>

Cart nes::cart;

/*
 * Just some thoughts on timing:
 * NTSC has a PPU/CPU ratio of 3.0/1.0
 * PAL  has a PPU/CPU ratio of 3.2/1.0
 *
 * If we want to smoothly support both, we need to implement
 * some sort of common cycle system. This will probably mean
 * adding some sort of "master cycle" system where:
 * 1 NTSC CPU cycle = 15 "master cycles"
 * 1 PAL  CPU cycle = 16 "master cycles"
 * 1 PPU      cycle =  5 "master cycles"
 *
 * using such a system would allow there to be a single absolute
 * timestamp system which would possibly be very beneficial
 */

namespace nes {

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void reset(Reset reset_type) {

	std::cout << "NES Reset Started" << std::endl;
	std::cout << "-----------------" << std::endl;
	cpu::reset(reset_type);
	ppu::reset(reset_type);
	apu::reset(reset_type);
	input::reset();
	std::cout << "-----------------" << std::endl;
	std::cout << "NES Reset Complete" << std::endl;
}

}
