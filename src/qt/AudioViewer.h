
#ifndef AUDIO_VIEWER_H_
#define AUDIO_VIEWER_H_

#include "ui_AudioViewer.h"
#include <QDialog>

class AudioViewer final : public QDialog {
	Q_OBJECT

public:
	AudioViewer(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected Q_SLOTS:
	bool eventFilter(QObject *watched, QEvent *event) override;

public:
	void setupUpdateTimer(QTimer *timer);

private:
	Ui::AudioViewer ui_;
};

#endif
