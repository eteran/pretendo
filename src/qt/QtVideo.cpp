
#include "QtVideo.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <immintrin.h>

// normalize the macros slightly
#if !defined(__AVX512F__) && defined(__AVX2__)
#define __AVX512F__
#endif

#if !defined(__SSE2__) && ((defined(_M_AMD64) || defined(_M_X64)) || (_M_IX86_FP == 2))
#define __SSE2__
#endif

//------------------------------------------------------------------------------
// Name: QtVideo
//------------------------------------------------------------------------------
QtVideo::QtVideo(QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags f)
	: QGLWidget(parent, shareWidget, f) {

	for (int i = 0; i < Height; ++i) {
		scanlines_[i] = &buffer_[i * Width];
	}

	setFormat(QGLFormat(QGL::DoubleBuffer));
	setMouseTracking(false);
	setBaseSize(Width, Height);

	std::cout << "[QtVideo::QtVideo]" << std::endl;
}

//------------------------------------------------------------------------------
// Name: resizeGL
//------------------------------------------------------------------------------
void QtVideo::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void QtVideo::initializeGL() {

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DITHER);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, Width);

	// clamp out of bounds texture coordinates
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// link the texture with the buffer
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, &buffer_[0]);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void QtVideo::paintGL() {

	const unsigned int w = width();
	const unsigned int h = height();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, &buffer_[0]);

	glBegin(GL_TRIANGLE_STRIP);
	/* clang-format off */
	glTexCoord2f(0.0, 0.0);	glVertex2f(0, h);
	glTexCoord2f(1.0, 0.0);	glVertex2f(w, h);
	glTexCoord2f(0.0, 1.0);	glVertex2f(0, 0);
	glTexCoord2f(1.0, 1.0);	glVertex2f(w, 0);
	/* clang-format on */
	glEnd();
}

//------------------------------------------------------------------------------
// Name: submit_scanline
//------------------------------------------------------------------------------
void QtVideo::submit_scanline(int scanline, const uint32_t *source) {
#if defined(__AVX512F__)
	auto s = reinterpret_cast<__m512i *>(scanlines_[scanline]);
	for (int i = 0; i < Width; i += 16) {
		auto ind = _mm512_loadu_si512(reinterpret_cast<const __m512i *>(source));
		auto vec = _mm512_i32gather_epi32(ind, reinterpret_cast<const __m512i *>(palette_), 4);
		*s++     = vec;
		source += 16;
	}

#elif defined(__AVX__)
	auto s = reinterpret_cast<__m256i *>(scanlines_[scanline]);
	for (int i = 0; i < Width; i += 8) {
		auto ind = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(source));
		auto vec = _mm256_i32gather_epi32(reinterpret_cast<const int *>(palette_), ind, 4);
		*s++     = vec;
		source += 8;
	}
#elif defined(__SSE2__)
	auto s = reinterpret_cast<__m128i *>(scanlines_[scanline]);
	for (int i = 0; i < Width; i += 4) {
		auto ind = _mm_loadu_si128(reinterpret_cast<const __m128i *>(source));
		auto vec = _mm_i32gather_epi32(reinterpret_cast<const int *>(palette_), ind, 4);
		*s++     = vec;
		source += 4;
	}
#else
	uint32_t *const s = scanlines_[scanline];
	std::transform(source, source + Width, s, [this](uint32_t index) {
		return palette_[index];
	});
#endif
}

//------------------------------------------------------------------------------
// Name: set_palette
//------------------------------------------------------------------------------
void QtVideo::set_palette(const color_emphasis_t *intensity, const rgb_color_t *pal) {
	assert(pal);
	assert(intensity);

	std::cout << "Setting Palette" << std::endl;

	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 64; i++) {
			palette_[j * 64 + i] =
				QColor(
					qBound(0x00, static_cast<int>(pal[i].r * intensity[j].r), 0xff),
					qBound(0x00, static_cast<int>(pal[i].g * intensity[j].g), 0xff),
					qBound(0x00, static_cast<int>(pal[i].b * intensity[j].b), 0xff))
					.rgb();
		}
	}
}

//------------------------------------------------------------------------------
// Name: end_frame
//------------------------------------------------------------------------------
void QtVideo::end_frame() {
	update();
}

//------------------------------------------------------------------------------
// Name: screenshot
//------------------------------------------------------------------------------
QImage QtVideo::screenshot() {
	QImage screen(Width, Height, QImage::Format_ARGB32);
	for (int i = 0; i < Height; ++i) {

		auto scanline = reinterpret_cast<QRgb *>(screen.scanLine(i));
		std::transform(scanlines_[i], scanlines_[i] + Width, scanline, [](uint32_t value) {
			return QRgb(value);
		});
	}
	return screen;
}
