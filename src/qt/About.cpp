
#include "About.h"

//------------------------------------------------------------------------------
// Name: About
//------------------------------------------------------------------------------
About::About(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f) {
	ui_.setupUi(this);
	setFixedSize(width(), height());
}
