
#include "QtVideo.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_2_1>
#include <QOpenGLVersionFunctionsFactory>
#include <QMutexLocker>
#include <algorithm>
#include <cassert>
#include <iostream>
#if defined(__AVX512F__) || defined(__AVX2__)
#include <immintrin.h>
#endif

//------------------------------------------------------------------------------
// Name: QtVideo
//------------------------------------------------------------------------------
QtVideo::QtVideo(QWidget *parent, Qt::WindowFlags f)
	: QOpenGLWidget(parent, f) {

	for (int i = 0; i < Height; ++i) {
		scanlines_[i] = &buffer_[i * Width];
	}

	// start from the application default so settings established there, such as
	// main()'s setSwapInterval(0), are not silently dropped
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setVersion(2, 1);
	setFormat(format);

	setMouseTracking(false);
	setBaseSize(Width, Height);

	std::cout << "[QtVideo::QtVideo]" << std::endl;
}

//------------------------------------------------------------------------------
// Name: resizeGL
//------------------------------------------------------------------------------
void QtVideo::resizeGL(int width, int height) {
	auto f = gl_functions();

	f->glViewport(0, 0, width, height);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void QtVideo::initializeGL() {

	auto f = gl_functions();

	f->initializeOpenGLFunctions();

	f->glDisable(GL_ALPHA_TEST);
	f->glDisable(GL_BLEND);
	f->glDisable(GL_DEPTH_TEST);
	f->glDisable(GL_POLYGON_SMOOTH);
	f->glDisable(GL_STENCIL_TEST);
	f->glEnable(GL_DITHER);
	f->glEnable(GL_TEXTURE_2D);
	f->glClearColor(0.0, 0.0, 0.0, 0.0);

	f->glGenTextures(1, &texture_);
	f->glBindTexture(GL_TEXTURE_2D, texture_);
	f->glPixelStorei(GL_UNPACK_ROW_LENGTH, Width);

	// clamp out of bounds texture coordinates
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// link the texture with the buffer
	f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, &buffer_[0]);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void QtVideo::paintGL() {

	const unsigned int w = width();
	const unsigned int h = height();

	auto f = gl_functions();

	f->glMatrixMode(GL_PROJECTION);
	f->glLoadIdentity();
	f->glOrtho(0, w, 0, h, -1.0, 1.0);

	f->glMatrixMode(GL_MODELVIEW);
	f->glLoadIdentity();

	f->glBindTexture(GL_TEXTURE_2D, texture_);
	{
		QMutexLocker lock(&buffer_mutex_);
		f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, &buffer_[0]);
	}

	f->glBegin(GL_TRIANGLE_STRIP);
	/* clang-format off */
	f->glTexCoord2f(0.0, 0.0);	f->glVertex2f(0, h);
	f->glTexCoord2f(1.0, 0.0);	f->glVertex2f(w, h);
	f->glTexCoord2f(0.0, 1.0);	f->glVertex2f(0, 0);
	f->glTexCoord2f(1.0, 1.0);	f->glVertex2f(w, 0);
	/* clang-format on */
	f->glEnd();
}

//------------------------------------------------------------------------------
// Name: submit_scanline
//------------------------------------------------------------------------------
void QtVideo::submit_scanline(int scanline, const uint32_t *source) {
	QMutexLocker lock(&buffer_mutex_);

	// these are gated on the ISA macros the compiler defines only when the
	// build actually targets that instruction set (-mavx2, /arch:AVX2), not on
	// QT_COMPILER_SUPPORTS_*, which merely says the compiler is capable of
	// emitting it and would need a runtime qCpuHasFeature() check to be safe
#if defined(__AVX512F__)
	auto s = reinterpret_cast<__m512i *>(scanlines_[scanline]);
	for (int i = 0; i < Width; i += 16) {
		auto ind = _mm512_loadu_si512(reinterpret_cast<const __m512i *>(source));
		auto vec = _mm512_i32gather_epi32(ind, reinterpret_cast<const __m512i *>(palette_), 4);
		*s++     = vec;
		source += 16;
	}
#elif defined(__AVX2__)
	auto s = reinterpret_cast<__m256i *>(scanlines_[scanline]);
	for (int i = 0; i < Width; i += 8) {
		auto ind = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(source));
		auto vec = _mm256_i32gather_epi32(reinterpret_cast<const int *>(palette_), ind, 4);
		*s++     = vec;
		source += 8;
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
	QMutexLocker lock(&buffer_mutex_);

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
	QMutexLocker lock(&buffer_mutex_);

	QImage screen(Width, Height, QImage::Format_ARGB32);
	for (int i = 0; i < Height; ++i) {

		auto scanline = reinterpret_cast<QRgb *>(screen.scanLine(i));
		std::transform(scanlines_[i], scanlines_[i] + Width, scanline, [](uint32_t value) {
			return QRgb(value);
		});
	}
	return screen;
}
