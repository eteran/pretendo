
#include "Preferences.h"
#include "../Palette.h"
#include <QPainter>

namespace {
	const int palette_block_width  = 24;
	const int palette_block_height = 12;
}


//------------------------------------------------------------------------------
// Name: Preferences
//------------------------------------------------------------------------------
Preferences::Preferences(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {
	ui_.setupUi(this);
	ui_.palette->installEventFilter(this);
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


		const rgb_color_t *const palette = Palette::NTSCPalette(
			Palette::default_saturation,
			Palette::default_hue,
			Palette::default_contrast,
			Palette::default_brightness,
			Palette::default_gamma
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
