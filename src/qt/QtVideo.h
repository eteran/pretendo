
#ifndef QGLVIDEO_20080314_H_
#define QGLVIDEO_20080314_H_

#include "Palette.h"
#include "VideoInterface.h"
#include <QGLWidget>
#include <memory>

class QtVideo final : public QGLWidget, public VideoInterface {
	Q_OBJECT
public:
	QtVideo(QWidget *parent = nullptr, const QGLWidget *shareWidget = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QtVideo() override = default;

public:
	// required functions
	void submit_scanline(int scanline, int intensity, const uint8_t *source) override;
	void set_palette(const color_emphasis_t *intensity, const rgb_color_t *pal) override;
	void end_frame() override;

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

Q_SIGNALS:
	void render_frame();

private:
	uint32_t*                   scanlines_[240];
	uint32_t                    palette_[8][64];
	std::unique_ptr<uint32_t[]> buffer_;
	GLuint                      texture_ = 0;
};

#endif
