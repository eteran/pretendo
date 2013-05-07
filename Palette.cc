
#include "Palette.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
template<class T>
const T &bound(const T &lower, const T &value, const T &upper) {
	using std::min;
	using std::max;

	return max(lower, min(value, upper));
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
int wave(int p, int color) {
	return (color + p + 8) % 12 < 6;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
float gamma_fix(float f, float gamma) {
	return f < 0.f ? 0.f : std::pow(f, 2.2f / gamma);
}

}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
rgb_color_t make_rgb_color(uint16_t pixel) {

	// The input value is a NES color index (with de-emphasis bits).
	// We need RGB values. Convert the index into RGB.
	// For most part, this process is described at:
	//    http://wiki.nesdev.com/w/index.php/NTSC_video
	// Decode the color index
	int color = (pixel & 0x0F);
	int level = color < 0xE ? (pixel >> 4) & 3 : 1;

	// Voltage levels, relative to synch voltage
	static const float black       = 0.518f;
	static const float white       = 1.962f;
	static const float attenuation = 0.746f;

	static const float levels[8] = {
		0.350f, 0.518f, 0.962f, 1.550f, // Signal low
		1.094f, 1.506f, 1.962f, 1.962f  // Signal high
	};

	const float lo_and_hi[2] = {
		levels[level + 4 * (color == 0x0)],
		levels[level + 4 * (color <  0xD)]
	};

	// Calculate the luma and chroma by emulating the relevant circuits:
	float y     = 0.f;
	float i     = 0.f;
	float q     = 0.f;
	float gamma = 1.8f;

	// 12 clock cycles per pixel.
	for(int p = 0; p < 12; ++p) {
		// NES NTSC modulator (square wave between two voltage levels):
		float spot = lo_and_hi[wave(p, color)];

		// De-emphasis bits attenuate a part of the signal:
		if(((pixel & 0x40) && wave(p, 12)) || ((pixel & 0x80) && wave(p, 4)) || ((pixel & 0x100) && wave(p, 8))) {
			spot *= attenuation;
		}

		// Normalize:
		const float v = (spot - black) / (white - black) / 12.f;

		// Ideal TV NTSC demodulator:
		y += v;
		i += v * std::cos(M_PI * p / 6);
		q += v * std::sin(M_PI * p / 6); // Or cos(... p-3 ... )
		// Note: Integrating cos() and sin() for p - 0.5 .. p + 0.5 range gives
		//       the exactly same result, scaled by a factor of 2*cos(pi/12).
	}

	// Convert YIQ into RGB according to FCC-sanctioned conversion matrix.
	rgb_color_t rgb;
	rgb.r = bound(0x00, static_cast<int>(255 * gamma_fix(y +  0.946882f * i +  0.623557f * q, gamma)), 0xff);
	rgb.g = bound(0x00, static_cast<int>(255 * gamma_fix(y + -0.274788f * i + -0.635691f * q, gamma)), 0xff);
	rgb.b = bound(0x00, static_cast<int>(255 * gamma_fix(y + -1.108545f * i +  1.709007f * q, gamma)), 0xff);
	return rgb;
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
rgb_color_t *Palette::NTSCPalette(double hue, double tint) {

	(void)hue;
	(void)tint;

	static rgb_color_t color_list[64];
	(void)hue;
	(void)tint;

#if 0
	static const double br1[] = { 0.50, 0.75, 1.00, 1.00 };
	static const double br2[] = { 0.29, 0.45, 0.73, 0.90 };
	static const double br3[] = { 0.00, 0.24, 0.47, 0.77 };

	static const double angles[] = {
		0.0, 	240.0, 	210.0,	180.0,
		150.0, 	120.0, 	90.0, 	60.0,
		30.0, 	0.0, 	330.0, 	300.0,
		270.0, 	0.0, 	0.0, 	0.0
	};

	double theta;
	double r;
	double g;
	double b;

	for(int x = 0; x < 4; x++) {
		for(int z = 0; z < 16; z++) {
			double s = tint;
			double y = br2[x];

			if(z == 0) {
				s = 0;
				y = br1[x];
			}

			if(z == 13) {
				s = 0;
				y = br3[x];
			}

			if(z == 14) {
				y = 0;
				s = 0;
			}

			if(z == 15) {
				y = 0;
				s = 0;
			}

			theta = M_PI * ((angles[z]) + hue) / 180.0;
			r = y + s * sin(theta);
			g = y - (27.0 / 53.0) * s * sin(theta) + (10.0 / 53.0) * s * cos(theta);
			b = y - s * cos(theta);

			r *= 256.0;
			g *= 256.0;
			b *= 256.0;

			r = bound(0.0, r, 255.0);
			g = bound(0.0, g, 255.0);
			b = bound(0.0, b, 255.0);

            color_list[x * 16 + z].r = static_cast<uint8_t>(static_cast<long int>(r + 0.5) & 0xff);
            color_list[x * 16 + z].g = static_cast<uint8_t>(static_cast<long int>(g + 0.5) & 0xff);
            color_list[x * 16 + z].b = static_cast<uint8_t>(static_cast<long int>(b + 0.5) & 0xff);
		}
	}
#else
	for(int i = 0; i < 64; ++i) {
		color_list[i] = make_rgb_color(i);
	}
#endif
	return color_list;
}


