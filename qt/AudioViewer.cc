
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
	ui_.widget->resize(APU::buffer_size, ui_.widget->height());
	
}

//------------------------------------------------------------------------------
// Name: ~Preferences
//------------------------------------------------------------------------------
AudioViewer::~AudioViewer() {

}

//------------------------------------------------------------------------------
// Name: update
//------------------------------------------------------------------------------
void AudioViewer::update() {
	using std::memcpy;
	if(Pretendo *p = qobject_cast<Pretendo *>(parent())) {
		memcpy(audio_buffer_, p->audio_buffer_, sizeof(audio_buffer_));
		emit ui_.widget->repaint();
	}
}


//------------------------------------------------------------------------------
// Name: eventFilter
//------------------------------------------------------------------------------
bool AudioViewer::eventFilter(QObject *watched, QEvent *event) {

	if (watched == ui_.widget && event->type() == QEvent::Paint) {
		QPainter painter;
		painter.begin(ui_.widget);

		// draw stuff...
		painter.fillRect(0, 0, ui_.widget->width(), ui_.widget->height(), Qt::blue);
		painter.setPen(Qt::yellow);
		int prev = -1;
		for(size_t i = 0; i < sizeof(audio_buffer_); ++i) {
			if(prev == -1) {
				painter.drawPoint(i, audio_buffer_[i]);
			} else {
				painter.drawLine(i - 1, prev, i, audio_buffer_[i]);
			}
			prev = audio_buffer_[i];
		}
		
		painter.end();
		return true;
	}
	return false;
}
