
#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <QDialog>
#include "ui_Preferences.h"

class Preferences : public QDialog {
	Q_OBJECT
public:
	Preferences(QWidget *parent = 0, Qt::WindowFlags f = 0);
	virtual ~Preferences();
	
protected Q_SLOTS:
	bool eventFilter(QObject *watched, QEvent *event);
	
private:
	Ui::Preferences ui_;
};

#endif
