
#ifndef PRETENDO_20120301_H_
#define PRETENDO_20120301_H_

#include <QMainWindow>
#include <QTime>
#include "ui_Pretendo.h"

class QAudioOutput;
class QFileSystemModel;
class QLabel;
class QSortFilterProxyModel;
class QTimer;
class Audio;

class Pretendo : public QMainWindow {
	Q_OBJECT

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

protected:
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void showEvent(QShowEvent *event);

private:
	Ui::Pretendo           ui_;
	QFileSystemModel      *filesystem_model_;
	QSortFilterProxyModel *filter_model_;
	QTimer                *timer_;
	QLabel                *fps_label_;
	QTime                  time_;
	quint64                framecount_;
	bool                   paused_;
	Audio                 *audio_;
};

#endif

