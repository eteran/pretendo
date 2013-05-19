
#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <QDialog>
#include "ui_Preferences.h"

class Preferences : public QDialog {
	Q_OBJECT
public:
	Preferences(QWidget *parent = 0, Qt::WindowFlags f = 0);
	virtual ~Preferences();
	
private:
	Ui::Preferences ui_;
};

#endif
