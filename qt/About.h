
#ifndef ABOUT_20130520_H_
#define ABOUT_20130520_H_

#include <QDialog>
#include "ui_About.h"

class About : public QDialog {
	Q_OBJECT
public:
	About(QWidget *parent = 0, Qt::WindowFlags f = 0);
	virtual ~About();
	
private:
	Ui::About ui_;
};

#endif
