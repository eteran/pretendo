
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

		QPixmap back_buffer(800, 128);
		QPainter painter;

		uint8_t samples[1024];
		size_t count = nes::apu::read_samples(samples, sizeof(samples));

		float sample_width = float(count) / 100.0;
		QPointF prev;

		if (painter.begin(&back_buffer)) {

			// draw stuff...
			painter.fillRect(back_buffer.rect(), Qt::black);

			for (size_t i = 0; i < count; ++i) {

				QPointF curr(i * sample_width, back_buffer.height() - samples[i]);

				if (prev.isNull()) {
					prev = curr;
				}

				if (curr.y() == prev.y()) {
					painter.setPen(Qt::white);
				} else if (curr.y() > prev.y()) {
					painter.setPen(Qt::green);
				} else {
					painter.setPen(Qt::red);
				}

				painter.drawLine(prev, curr);
				prev = curr;
			}

			painter.end();
		}

		painter.begin(ui_.widget);
		painter.drawPixmap(ui_.widget->rect(), back_buffer);
		return true;
	}
	return false;
}
