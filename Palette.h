
#ifndef PALETTE_20080314_H_
#define PALETTE_20080314_H_

#include <boost/cstdint.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

struct rgb_color_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct color_emphasis_t {
	double r;
	double g;
	double b;
};


namespace Palette {

	static const float default_saturation = 1.0f;
	static const float default_hue        = 0.0f;
	static const float default_contrast   = 1.0f;
	static const float default_brightness = 1.0f;
	static const float default_gamma      = 1.8f;

	rgb_color_t *NTSCPalette(float saturation, float hue, float contrast, float brightness, float gamma);

	/*
	   000 = No Effect
	   001 = Emphasize Red (or De-Emphasize Cyan)
	   010 = Emphasize Green (or De-Emphasize Magenta)
	   011 = Emphasize Yellow (or De-Emphasize Blue)
	   100 = Emphasize Blue (or De-Emphasize Yellow)
	   101 = Emphasize Magenta (or De-Emphasize Green)
	   110 = Emphasize Cyan (or De-Emphasize Red)
	   111 = Emphasize Black (or De-Emphasize Red, Green, and Blue)

	Here are some *average* amounts showing the change in the RGB components of a
	neutral color (ie: grey or white) when a certain color is being emphasized
	through $2001.
	*/

	const color_emphasis_t intensity[8] = {
		{ 1.000, 1.000, 1.000 },
		{ 1.239, 0.915, 0.743 },
		{ 0.794, 1.086, 0.882 },
		{ 0.905, 1.026, 1.277 },
		{ 0.741, 0.987, 1.001 },
		{ 1.023, 0.908, 0.979 },
		{ 1.019, 0.980, 0.653 },
		{ 0.750, 0.750, 0.750 }
	};
}

#endif
