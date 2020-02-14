
#ifndef NES_H_
#define NES_H_

#include "Ppu.h"
#include "Reset.h"

class Cart;
class Config;

namespace nes {

extern Cart cart;

namespace {
uint8_t buffer[256];
}

namespace detail {

//------------------------------------------------------------------------------
// Name: execute_scanline_0_19
//------------------------------------------------------------------------------
inline void execute_scanline_0_19() {

	/*
	 * 0..19:  Starting at the instant the VINT flag is pulled down (when a NMI is
	 * generated), 20 scanlines make up the period of time on the PPU which I like
	 * to call the VINT period. During this time, the PPU makes no access to it's
	 * external memory (i.e. name / pattern tables, etc.).
	 */

	for (int i = 0; i < 20; ++i) {
		ppu::execute_scanline(scanline_vblank());
	}
}

//------------------------------------------------------------------------------
// Name: execute_scanline_20
//------------------------------------------------------------------------------
inline void execute_scanline_20() {

	/*
	 * 20:  After 20 scanlines worth of time go by (since the VINT flag was set),
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
	ppu::start_frame();
	ppu::execute_scanline(scanline_prerender());
}

//------------------------------------------------------------------------------
// Name: execute_scanline_21_260
//------------------------------------------------------------------------------
template <class Video>
void execute_scanline_21_260(Video *video) {

	/*
	 * 21..260: after rendering 1 dummy scanline, the PPU starts to render the
	 * actual data to be displayed on the screen. This is done for 240 scanlines,
	 * of course.
	 */

	// process the visible range
	for (int i = 0; i < 240; ++i) {
		ppu::execute_scanline(scanline_render(buffer));
		video->submit_scanline(i, ppu::mask().intensity, buffer);
	}
}

//------------------------------------------------------------------------------
// Name: execute_scanline_261
//------------------------------------------------------------------------------
inline void execute_scanline_261() {

	/*
	 * 261:  after the very last rendered scanline finishes, the PPU does nothing
	 * for 1 scanline (i.e. the programmer gets screwed out of perfectly good VINT
	 * time). When this scanline finishes, the VINT flag is set, and the process of
	 * drawing lines starts all over again.
	 */

	ppu::end_frame();
	ppu::execute_scanline(scanline_postrender());
}
}

//------------------------------------------------------------------------------
// Name: run_frame
//------------------------------------------------------------------------------
template <class Video>
void run_frame(Video *video) {
	detail::execute_scanline_20();
	detail::execute_scanline_21_260(video);
	detail::execute_scanline_261();
	detail::execute_scanline_0_19();
}

void reset(Reset reset_type);

}

#endif
