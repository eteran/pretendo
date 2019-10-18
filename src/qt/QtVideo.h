
#ifndef QGLVIDEO_20080314_H_
#define QGLVIDEO_20080314_H_

#include "Palette.h"
#include <QGLWidget>
#include <array>

class QtVideo final : public QGLWidget {
	Q_OBJECT
private:
	static constexpr int Width  = 256;
	static constexpr int Height = 240;
public:
	QtVideo(QWidget *parent = nullptr, const QGLWidget *shareWidget = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QtVideo() override = default;

public:
	// required functions
	void submit_scanline(int scanline, int intensity, const uint8_t *source);
	void set_palette(const color_emphasis_t *intensity, const rgb_color_t *pal);
	void end_frame();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

Q_SIGNALS:
	void render_frame();

private:
	uint32_t*                            scanlines_[240];
	uint32_t                             palette_[8][64];
	std::array<uint32_t, Width * Height> buffer_;
	GLuint                               texture_ = 0;
};

#endif
