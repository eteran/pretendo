
#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <QDialog>
#include "ui_Preferences.h"

class Preferences : public QDialog {
	Q_OBJECT
public:
	Preferences(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~Preferences();
	
protected Q_SLOTS:
	bool eventFilter(QObject *watched, QEvent *event);
	void on_hue_valueChanged(int value);
	void on_saturation_valueChanged(int value);
	void on_contrast_valueChanged(int value);
	void on_brightness_valueChanged(int value);
	void on_gamma_valueChanged(int value);
	void on_buttonBox_clicked(QAbstractButton *button);
	void on_buttonBox_accepted();
	
private:
	Ui::Preferences ui_;
	float current_saturation_;
	float current_hue_;
	float current_contrast_;
	float current_brightness_;
	float current_gamma_;
};

#endif
