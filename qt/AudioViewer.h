
#ifndef AUDIO_VIEWER_H_
#define AUDIO_VIEWER_H_

#include <QDialog>
#include <boost/cstdint.hpp>
#include "APU.h"
#include "ui_AudioViewer.h"

class AudioViewer : public QDialog {
	Q_OBJECT

public:
	AudioViewer(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~AudioViewer();

protected Q_SLOTS:
	bool eventFilter(QObject *watched, QEvent *event);

public Q_SLOTS:
	void update();

private:
	Ui::AudioViewer ui_;
	uint8_t audio_buffer_[APU::buffer_size];
};

#endif
