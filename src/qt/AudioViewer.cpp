
#include "AudioViewer.h"
#include "Apu.h"
#include "Dmc.h"
#include "Noise.h"
#include "Square.h"
#include "Triangle.h"
#include <QPainter>
#include <QTimer>

namespace {

void drawBar(QPainter *painter, int index, int value) {

	QRect rect;
	rect.setLeft((index * 20) + 10);
	rect.setBottom(256);
	rect.setWidth(10);
	rect.setTop(256 - value * 3);

	QLinearGradient gradient(rect.topLeft(), rect.bottomRight()); // diagonal gradient from top-left to bottom-right
	gradient.setColorAt(1, Qt::green);
	gradient.setColorAt(0, Qt::red);
	painter->fillRect(rect, gradient);
}

}

//------------------------------------------------------------------------------
// Name: AudioViewer
//------------------------------------------------------------------------------
AudioViewer::AudioViewer(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f) {

	ui_.setupUi(this);
	ui_.widget->installEventFilter(this);
	ui_.widget->resize(nes::apu::buffer_size, ui_.widget->height());
}

//------------------------------------------------------------------------------
// Name: setupUpdateTimer
//------------------------------------------------------------------------------
void AudioViewer::setupUpdateTimer(QTimer *timer) {
	connect(timer, &QTimer::timeout, this, [this]() {
		ui_.widget->update();
	});
}

//------------------------------------------------------------------------------
// Name: eventFilter
//------------------------------------------------------------------------------
bool AudioViewer::eventFilter(QObject *watched, QEvent *event) {

	if (watched == ui_.widget && event->type() == QEvent::Paint) {

		const int pulse1_out   = nes::apu::square_0.output();
		const int pulse2_out   = nes::apu::square_1.output();
		const int triangle_out = nes::apu::triangle.output();
		const int noise_out    = nes::apu::noise.output();
		const int dmc_out      = nes::apu::dmc.output();

		QPixmap back_buffer(110, 256);
		QPainter painter;
		if (painter.begin(&back_buffer)) {

			// draw stuff...
			painter.fillRect(back_buffer.rect(), Qt::black);

			drawBar(&painter, 0, pulse1_out);
			drawBar(&painter, 1, pulse2_out);
			drawBar(&painter, 2, triangle_out);
			drawBar(&painter, 3, noise_out);
			drawBar(&painter, 4, dmc_out);

			painter.end();
		}

		painter.begin(ui_.widget);
		painter.drawPixmap(ui_.widget->rect(), back_buffer);
		return true;
	}
	return false;
}
