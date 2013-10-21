
#include "NES.h"
#include "Mapper.h"
#include "VideoInterface.h"

CPU    nes::cpu;
PPU    nes::ppu;
APU    nes::apu;
Cart   nes::cart;
Input  nes::input;
Config nes::config;

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

namespace {

//------------------------------------------------------------------------------
// Name: execute_scanline_0_19
//------------------------------------------------------------------------------
void execute_scanline_0_19() {

	/*
	 * 0..19:	Starting at the instant the VINT flag is pulled down (when a NMI is
	 * generated), 20 scanlines make up the period of time on the PPU which I like
	 * to call the VINT period. During this time, the PPU makes no access to it's
	 * external memory (i.e. name / pattern tables, etc.).
	 */

	for(int i = 0; i < 20; ++i) {
		nes::ppu.execute_scanline(scanline_vblank());
	}
}

//------------------------------------------------------------------------------
// Name: execute_scanline_20
//------------------------------------------------------------------------------
void execute_scanline_20() {

	/*
	 * 20:	After 20 scanlines worth of time go by (since the VINT flag was set),
	 * the PPU starts to render scanlines. This first scanline is a dummy one;
	 * although it will access it's external memory in the same sequence it would
	 * for drawing a valid scanline, no on-screen pixels are rendered during this
	 * time, making the fetched background data immaterial. Both horizontal *and*
	 * vertical scroll counters are updated (presumably) at cc offset 256 in this
	 * scanline. Other than that, the operation of this scanline is identical to
	 * any other. The primary reason this scanline exists is to start the object
	 * render pipeline, since it takes 256 cc's worth of time to determine which
	 * objects are in range or not for any particular scanline.
	 *
	 * IMPORTANT! this is the only scanline that has variable length. On every
	 * second rendered frame, this scanline is only 1360 cycles. Otherwise it's
	 * 1364.
	 */

	nes::ppu.execute_scanline(scanline_prerender());
}

//------------------------------------------------------------------------------
// Name: execute_scanline_21_260
//------------------------------------------------------------------------------
void execute_scanline_21_260(VideoInterface *video) {

	/*
	 * 21..260: after rendering 1 dummy scanline, the PPU starts to render the
	 * actual data to be displayed on the screen. This is done for 240 scanlines,
	 * of course.
	 */

	static uint8_t buffer[256];

	// process the visible range
	for(int i = 0; i < 240; ++i) {
		nes::ppu.execute_scanline(scanline_render(buffer));
		video->submit_scanline(i, nes::ppu.color_intensity(), buffer);
	}
}

//------------------------------------------------------------------------------
// Name: execute_scanline_261
//------------------------------------------------------------------------------
void execute_scanline_261() {

	/*
	 * 261:	after the very last rendered scanline finishes, the PPU does nothing
	 * for 1 scanline (i.e. the programmer gets screwed out of perfectly good VINT
	 * time). When this scanline finishes, the VINT flag is set, and the process of
	 * drawing lines starts all over again.
	 */

	nes::ppu.execute_scanline(scanline_postrender());
}
}

//------------------------------------------------------------------------------
// Name: run_frame
//------------------------------------------------------------------------------
void nes::run_frame(VideoInterface *video) {
	nes::ppu.start_frame();
	execute_scanline_20();
	execute_scanline_21_260(video);
	nes::ppu.end_frame();
	execute_scanline_261();
	execute_scanline_0_19();
}

//------------------------------------------------------------------------------
// Name: reset
//------------------------------------------------------------------------------
void nes::reset(nes::RESET reset_type) {

	nes::cpu.reset(reset_type);
	nes::ppu.reset(reset_type);
	nes::apu.reset(reset_type);
	nes::input.reset();
}
