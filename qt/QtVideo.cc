
#include "QtVideo.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#ifdef _WIN32
#include "glext.h"
#endif

namespace {

struct palette_entry {
	palette_entry(const uint32_t *p) : palette_(p) {
	}

	uint32_t operator()(uint8_t index) const {
		return palette_[index];
	}

private:
	const uint32_t *const palette_;
};

const int WIDTH  = 256;
const int HEIGHT = 240;

}

//------------------------------------------------------------------------------
// Name: QtVideo
//------------------------------------------------------------------------------
QtVideo::QtVideo(QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags f) : QGLWidget(parent, shareWidget, f), buffer_(0), texture_(0) {

	buffer_ = new uint32_t[WIDTH * HEIGHT]();
	for(int i = 0; i < 240; ++i) {
		scanlines_[i] = &buffer_[i * WIDTH];
	}

	setAutoBufferSwap(true);
	setFormat(QGLFormat(QGL::DoubleBuffer));
	setMouseTracking(false);
	setBaseSize(WIDTH, HEIGHT);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	connect(this, SIGNAL(render_frame()), this, SLOT(updateGL()));

	std::cout << "[QtVideo::QtVideo]" << std::endl;
}

//------------------------------------------------------------------------------
// Name: ~QtVideo
//------------------------------------------------------------------------------
QtVideo::~QtVideo() {
	delete [] buffer_;
}

//------------------------------------------------------------------------------
// Name: resizeGL
//------------------------------------------------------------------------------
void QtVideo::resizeGL(int width, int height) {
	Q_UNUSED(width);
	Q_UNUSED(height);
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
	glPixelStorei(GL_UNPACK_ROW_LENGTH, WIDTH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, buffer_);
}

//------------------------------------------------------------------------------
// Name: submit_scanline
//------------------------------------------------------------------------------
void QtVideo::submit_scanline(int scanline, int intensity, const uint8_t *source) {
	uint32_t *const s = scanlines_[scanline];
	std::transform(source, source + 256, s, palette_entry(palette_[intensity]));
}

//------------------------------------------------------------------------------
// Name: set_palette
//------------------------------------------------------------------------------
void QtVideo::set_palette(const color_emphasis_t *intensity, const rgb_color_t *pal) {
	assert(pal != 0);
	assert(intensity != 0);

	for(int j = 0; j < 8; j++) {
		for(int i = 0; i < 64; i++) {
			palette_[j][i] =
				QColor(
					qBound(0x00, static_cast<int>(pal[i].r * intensity[j].r), 0xff),
					qBound(0x00, static_cast<int>(pal[i].g * intensity[j].g), 0xff),
					qBound(0x00, static_cast<int>(pal[i].b * intensity[j].b), 0xff)).rgb();

		}
	}
}

//------------------------------------------------------------------------------
// Name: start_frame
//------------------------------------------------------------------------------
void QtVideo::start_frame() {
}

//------------------------------------------------------------------------------
// Name: end_frame
//------------------------------------------------------------------------------
void QtVideo::end_frame() {
	emit render_frame();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void QtVideo::paintGL() {

	const unsigned output_width  = width();
	const unsigned output_height = height();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, output_width, 0, output_height, -1.0, 1.0);
	glViewport(0, 0, output_width, output_height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (/*filter == Video::FilterPoint*/ false) ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (/*filter == Video::FilterPoint*/ false) ? GL_NEAREST : GL_LINEAR);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, buffer_);

	const unsigned u = output_width;
	const unsigned v = output_height;

	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.0, 0.0); glVertex3i(0, v, 0);
	glTexCoord2f(1.0, 0.0); glVertex3i(u, v, 0);
	glTexCoord2f(0.0, 1.0); glVertex3i(0, 0, 0);
	glTexCoord2f(1.0, 1.0); glVertex3i(u, 0, 0);
	glEnd();
}
