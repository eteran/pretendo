
#ifndef AUDIO_VIEWER_H_
#define AUDIO_VIEWER_H_

#include "APU.h"
#include <QDialog>
#include <cstdint>
#include "ui_AudioViewer.h"

class AudioViewer : public QDialog {
	Q_OBJECT

public:
	AudioViewer(QWidget *parent = nullptr, Qt::WindowFlags f = nullptr);

protected Q_SLOTS:
	bool eventFilter(QObject *watched, QEvent *event) override;

public Q_SLOTS:
	void update();

private:
	Ui::AudioViewer ui_;
};

#endif
