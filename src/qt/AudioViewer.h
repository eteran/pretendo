
#ifndef AUDIO_VIEWER_H_
#define AUDIO_VIEWER_H_

#include "Apu.h"
#include "ui_AudioViewer.h"
#include <QDialog>
#include <cstdint>

class AudioViewer final : public QDialog {
	Q_OBJECT

public:
	AudioViewer(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected Q_SLOTS:
	bool eventFilter(QObject *watched, QEvent *event) override;

public Q_SLOTS:
	void update();

private:
	Ui::AudioViewer ui_;
};

#endif
