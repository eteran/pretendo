
#ifndef VIDEOINTERFACE_20080314_H_
#define VIDEOINTERFACE_20080314_H_

#include "Palette.h"

class color_emphasis_t;
class rgb_color_t;

class VideoInterface {
public:
	virtual ~VideoInterface() {}
public:
	virtual void submit_scanline(int scanline, int intensity, const uint8_t *source) = 0;
	virtual void set_palette(const color_emphasis_t *intensity, const rgb_color_t *pal) = 0;
	virtual void start_frame() = 0;
	virtual void end_frame() = 0;
};

#endif
