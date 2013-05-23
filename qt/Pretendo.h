
#ifndef PRETENDO_20120301_H_
#define PRETENDO_20120301_H_

#include <QMainWindow>
#include <QTime>
#include "Preferences.h"
#include "ui_Pretendo.h"

class QAudioOutput;
class QFileSystemModel;
class QLabel;
class QSortFilterProxyModel;
class QTimer;

#if defined(ENABLE_SOUND) && defined(USE_QAUDIO)
	class QtAudio;
#elif defined(ENABLE_SOUND) && defined(USE_SDLAUDIO)
	class SDLAudio;
#elif defined(ENABLE_SOUND)
	class AlsaAudio;
#else
	class NullAudio;
#endif

class Pretendo : public QMainWindow {
	Q_OBJECT
	friend class Preferences;

public:
	Pretendo(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~Pretendo();

private Q_SLOTS:
	void picked(const QModelIndex &index);
	void update();
	void on_action_Load_ROM_triggered();
	void on_action_Free_ROM_triggered();
	void on_action_Run_triggered();
	void on_action_Stop_triggered();
	void on_action_Pause_triggered();
	void on_actionShow_Sprites_toggled(bool value);
	void on_action_Hard_Reset_triggered();
	void on_actionReset_triggered();
	void on_action1x_triggered();
	void on_action2x_triggered();
	void on_action3x_triggered();
	void on_action4x_triggered();
	void on_action_Preferences_triggered();
	void on_actionAbout_Qt_triggered();
	void on_action_About_triggered();

protected:
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void showEvent(QShowEvent *event);

private:
	Ui::Pretendo           ui_;
	Preferences           *preferences_;
	QFileSystemModel      *filesystem_model_;
	QSortFilterProxyModel *filter_model_;
	QTimer                *timer_;
	QLabel                *fps_label_;
	Qt::Key                player_1_[8];
	QTime                  time_;
	quint64                framecount_;
	bool                   paused_;
#if defined(ENABLE_SOUND) && defined(USE_QAUDIO)
	QtAudio *audio_;
#elif defined(ENABLE_SOUND) && defined(USE_SDLAUDIO)
	SDLAudio *audio_;
#elif defined(ENABLE_SOUND)
	AlsaAudio *audio_;
#else
	NullAudio *audio_;
#endif

};

#endif

