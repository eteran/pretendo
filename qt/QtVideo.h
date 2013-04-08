
#ifndef QGLVIDEO_20080314_H_
#define QGLVIDEO_20080314_H_

#include <QGLWidget>
#include "Palette.h"
#include "VideoInterface.h"

class Mediator;

class QtVideo : public QGLWidget, public VideoInterface {
	Q_OBJECT
public:
	QtVideo(QWidget *parent = 0, const QGLWidget *shareWidget = 0, Qt::WindowFlags f = 0);
	virtual ~QtVideo();

public:
	// required functions
	void submit_scanline(int scanline, int intensity, const uint8_t *source);
	void set_palette(const color_emphasis_t *intensity, const rgb_color_t *pal);
	void start_frame();
	void end_frame();

protected:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

Q_SIGNALS:
	void render_frame();

private:
	uint32_t  palette_[8][64];
	uint32_t *buffer_;
	GLuint    texture_;
	uint32_t *scanlines_[240];
};

#endif
