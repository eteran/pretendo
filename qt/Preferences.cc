
#include "Preferences.h"
#include "Pretendo.h"
#include "../Palette.h"
#include <QPainter>
#include <QAbstractButton>
#include <QDebug>

namespace {
	const int palette_block_width  = 24;
	const int palette_block_height = 12;
}

//------------------------------------------------------------------------------
// Name: Preferences
//------------------------------------------------------------------------------
Preferences::Preferences(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), 
		current_saturation_(Palette::default_saturation), 
		current_hue_(Palette::default_hue), 
		current_contrast_(Palette::default_contrast), 
		current_brightness_(Palette::default_brightness), 
		current_gamma_(Palette::default_gamma) {
		
	ui_.setupUi(this);
	ui_.palette->installEventFilter(this);
	
	ui_.hue       ->setValue(current_hue_        * 1000);
	ui_.saturation->setValue(current_saturation_ * 100);
	ui_.contrast  ->setValue(current_contrast_   * 100);
	ui_.brightness->setValue(current_brightness_ * 100);
	ui_.gamma     ->setValue(current_gamma_      * 100);
	
}

//------------------------------------------------------------------------------
// Name: ~Preferences
//------------------------------------------------------------------------------
Preferences::~Preferences() {

}

//------------------------------------------------------------------------------
// Name: eventFilter
//------------------------------------------------------------------------------
bool Preferences::eventFilter(QObject *watched, QEvent *event) {

	if (watched == ui_.palette && event->type() == QEvent::Paint) {
		QPainter painter;
		painter.begin(ui_.palette);

		const rgb_color_t *const palette = Palette::NTSC(
			current_saturation_,
			current_hue_,
			current_contrast_,
			current_brightness_,
			current_gamma_
		);
		
		for(int x = 0; x < 16; ++x) {
			for(int y = 0; y < 4; ++y) {
				const int index = (y * 16) + x;
				for(int intensity = 0; intensity < 8; ++intensity) {
					painter.fillRect(x * palette_block_width, y * palette_block_height + (palette_block_height * 4) * intensity, palette_block_width, palette_block_height, QColor(
						qBound<int>(0, palette[index].r * Palette::intensity[intensity].r, 255), 
						qBound<int>(0, palette[index].g * Palette::intensity[intensity].g, 255),
						qBound<int>(0, palette[index].b * Palette::intensity[intensity].b, 255)));
				}
			}
		}
		
		painter.end();
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
// Name: on_hue_valueChanged
//------------------------------------------------------------------------------
void Preferences::on_hue_valueChanged(int value) {
	current_hue_ = static_cast<float>(value) / 1000.0;
	ui_.palette->update();
}

//------------------------------------------------------------------------------
// Name: on_saturation_valueChanged
//------------------------------------------------------------------------------
void Preferences::on_saturation_valueChanged(int value) {
	current_saturation_ = static_cast<float>(value) / 100.0;
	ui_.palette->update();
}

//------------------------------------------------------------------------------
// Name: on_contrast_valueChanged
//------------------------------------------------------------------------------
void Preferences::on_contrast_valueChanged(int value) {
	current_contrast_ = static_cast<float>(value) / 100.0;
	ui_.palette->update();
}

//------------------------------------------------------------------------------
// Name: on_brightness_valueChanged
//------------------------------------------------------------------------------
void Preferences::on_brightness_valueChanged(int value) {
	current_brightness_ = static_cast<float>(value) / 100.0;
	ui_.palette->update();
}

//------------------------------------------------------------------------------
// Name: on_gamma_valueChanged
//------------------------------------------------------------------------------
void Preferences::on_gamma_valueChanged(int value) {
	current_gamma_ = static_cast<float>(value) / 100.0;
	ui_.palette->update();
}

//------------------------------------------------------------------------------
// Name: on_buttonBox_clicked
//------------------------------------------------------------------------------
void Preferences::on_buttonBox_clicked(QAbstractButton *button) {

	if(ui_.buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults) {
		ui_.hue       ->setValue(Palette::default_hue        * 1000);
		ui_.saturation->setValue(Palette::default_saturation * 100);
		ui_.contrast  ->setValue(Palette::default_contrast   * 100);
		ui_.brightness->setValue(Palette::default_brightness * 100);
		ui_.gamma     ->setValue(Palette::default_gamma      * 100);
	}
}

//------------------------------------------------------------------------------
// Name: on_buttonBox_accepted
//------------------------------------------------------------------------------
void Preferences::on_buttonBox_accepted() {
	if(Pretendo *const p = qobject_cast<Pretendo *>(parent())) {
		p->ui_.video->set_palette(Palette::intensity, Palette::NTSC(
			current_saturation_,
			current_hue_,
			current_contrast_,
			current_brightness_,
			current_gamma_));
	}
}
