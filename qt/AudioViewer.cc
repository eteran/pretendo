
#include "AudioViewer.h"
#include "Pretendo.h"
#include <QPainter>
#include <cstring>

//------------------------------------------------------------------------------
// Name: AudioViewer
//------------------------------------------------------------------------------
AudioViewer::AudioViewer(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {
		
	ui_.setupUi(this);
	ui_.widget->installEventFilter(this);
	ui_.widget->resize(nes::apu::buffer_size, ui_.widget->height());
	
	using std::memset;
	memset(audio_buffer_, 0, sizeof(audio_buffer_));
	
}

//------------------------------------------------------------------------------
// Name: ~AudioViewer
//------------------------------------------------------------------------------
AudioViewer::~AudioViewer() {

}

//------------------------------------------------------------------------------
// Name: update
//------------------------------------------------------------------------------
void AudioViewer::update() {
	using std::memcpy;
	if(Pretendo *const p = qobject_cast<Pretendo *>(parent())) {
		memcpy(audio_buffer_, p->audio_buffer_, sizeof(audio_buffer_));
		emit ui_.widget->repaint();
	}
}

//------------------------------------------------------------------------------
// Name: eventFilter
//------------------------------------------------------------------------------
bool AudioViewer::eventFilter(QObject *watched, QEvent *event) {

	if(watched == ui_.widget && event->type() == QEvent::Paint) {
	
		QPixmap back_buffer(sizeof(audio_buffer_), 300);
		QPainter painter;
		if(painter.begin(&back_buffer)) {	
			

			// draw stuff...
			painter.fillRect(back_buffer.rect(), Qt::blue);
			
			painter.setPen(Qt::red);
			painter.drawLine(0, 0xff, sizeof(audio_buffer_), 0xff);
			
			painter.setPen(Qt::yellow);
			int prev = -1;
			for(size_t i = 0; i < sizeof(audio_buffer_); ++i) {

				const int current_value = audio_buffer_[i];

				if(prev == -1) {
					painter.drawPoint(i, 0xff - current_value);
				} else {
					painter.drawLine(i - 1, 0xff - prev, i, 0xff - current_value);
				}
				
				prev = current_value;
			}
			painter.end();
		}
		
		painter.begin(ui_.widget);
		painter.drawPixmap(ui_.widget->rect(), back_buffer);
		return true;
	}
	return false;
}
