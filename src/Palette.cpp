
#include "Palette.h"
#include <algorithm>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {

//------------------------------------------------------------------------------
// Name: bound
//------------------------------------------------------------------------------
template <class T>
constexpr T bound(T lower, T value, T upper) {
	using std::max;
	using std::min;

	return max(lower, min(value, upper));
}

//------------------------------------------------------------------------------
// Name: wave
//------------------------------------------------------------------------------
constexpr int wave(int p, int color) {
	return (color + p + 8) % 12 < 6;
}

//------------------------------------------------------------------------------
// Name: gamma_fix
//------------------------------------------------------------------------------
constexpr float gamma_fix(float f, float gamma) {
	using std::pow;
	return f < 0.f ? 0.f : pow(f, 2.2f / gamma);
}

//------------------------------------------------------------------------------
// Name: make_rgb_color
//------------------------------------------------------------------------------
rgb_color_t make_rgb_color(uint16_t pixel, float saturation, float hue, float contrast, float brightness, float gamma) {

	// The input value is a NES color index (with de-emphasis bits).
	// We need RGB values. Convert the index into RGB.
	// For most part, this process is described at:
	//    http://wiki.nesdev.com/w/index.php/NTSC_video

	// Decode the color index
	const uint8_t color = (pixel & 0x0F);
	const uint8_t level = color < 0xE ? (pixel >> 4) & 3 : 1;

	// Voltage levels, relative to synch voltage
	static constexpr float black       = 0.518f;
	static constexpr float white       = 1.962f;
	static constexpr float attenuation = 0.746f;

	static const float levels[8] = {
		0.350f, 0.518f, 0.962f, 1.550f, // Signal low
		1.094f, 1.506f, 1.962f, 1.962f  // Signal high
	};

	const float lo_and_hi[2] = {
		levels[level + 4 * (color == 0x0)],
		levels[level + 4 * (color < 0xD)]};

	// Calculate the luma and chroma by emulating the relevant circuits:
	float y = 0.f;
	float i = 0.f;
	float q = 0.f;

	// 12 clock cycles per pixel.
	for (int p = 0; p < 12; ++p) {

		// NES NTSC modulator (square wave between two voltage levels):
		float spot = lo_and_hi[wave(p, color)];

		// De-emphasis bits attenuate a part of the signal:
		if (((pixel & 0x40) && wave(p, 12)) || ((pixel & 0x80) && wave(p, 4)) || ((pixel & 0x100) && wave(p, 8))) {
			spot *= attenuation;
		}

		// Normalize:
		float v = (spot - black) / (white - black);

		// Ideal TV NTSC demodulator:
		// Apply contrast/brightness
		v = (v - .5f) * contrast + .5f;
		v *= brightness / 12.f;

		y += v;
		i += v * std::cos((M_PI / 6.) * (p + hue));
		q += v * std::sin((M_PI / 6.) * (p + hue));
	}

	i *= saturation;
	q *= saturation;

	// Convert YIQ into RGB according to FCC-sanctioned conversion matrix.
	rgb_color_t rgb;
	rgb.r = bound(0x00, static_cast<int>(255 * gamma_fix(y + 0.946882f * i + 0.623557f * q, gamma)), 0xff);
	rgb.g = bound(0x00, static_cast<int>(255 * gamma_fix(y + -0.274788f * i + -0.635691f * q, gamma)), 0xff);
	rgb.b = bound(0x00, static_cast<int>(255 * gamma_fix(y + -1.108545f * i + 1.709007f * q, gamma)), 0xff);
	return rgb;
}

}

//------------------------------------------------------------------------------
// Name: NTSC
//------------------------------------------------------------------------------
const rgb_color_t *Palette::NTSC(float saturation, float hue, float contrast, float brightness, float gamma) {

	std::cout << "Creating Palette: <" << saturation << "," << hue << "," << contrast << "," << brightness << "," << gamma << ">" << std::endl;
	static rgb_color_t color_list[64];

	for (int i = 0; i < 64; ++i) {
		color_list[i] = make_rgb_color(i, saturation, hue, contrast, brightness, gamma);
	}

	return color_list;
}
