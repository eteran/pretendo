
#include "AudioViewer.h"
#include "Apu.h"
#include <QPainter>
#include <QTimer>

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

		QImage back_buffer(1024, 128, QImage::Format_RGB32);
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
					painter.setPen(Qt::blue);
				}

				painter.drawLine(prev, curr);
				prev = curr;
			}

			painter.end();
		}

		painter.begin(ui_.widget);
		painter.drawImage(ui_.widget->rect(), back_buffer);
		painter.end();
		return true;
	}
	return false;
}
