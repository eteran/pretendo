

#include "PatternTableView.h"
#include "Cart.h"
#include "Nes.h"
#include "Ppu.h"
#include <QImage>
#include <QPainter>
#include <QTimer>

//------------------------------------------------------------------------------
// Name: AudioViewer
//------------------------------------------------------------------------------
PatternTableView::PatternTableView(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f) {

	ui_.setupUi(this);
	ui_.patternTable0->installEventFilter(this);
	ui_.patternTable1->installEventFilter(this);
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PatternTableView::setupUpdateTimer(QTimer *timer) {

	connect(timer, &QTimer::timeout, this, [this]() {
		ui_.patternTable0->update();
		ui_.patternTable1->update();
	});
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PatternTableView::drawPatternTable8x8(int32_t which, QImage *img) {
	for (int32_t y = 0; y < 16; y++) {
		for (int32_t x = 0; x < 16; x++) {
			drawTile(which, x + (y * 16), x, y, img);
		}
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PatternTableView::drawPatternTable8x16(int32_t which, QImage *img) {
	int32_t x = 0;
	int32_t y = 0;

	for (int32_t i = 0; i < 8; ++i) {
		for (int32_t t = (i * 32); t < ((i * 32) + 32); t += 2) {
			drawTile(which, t, x, y, img);
			drawTile(which, (t + 1), x, (y + 1), img);
			++x;
		}

		y += 2;
		x = 0;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PatternTableView::drawTile(int32_t patternTable, int32_t tileIndex, int32_t tileX, int32_t tileY, QImage *img) {
	int32_t shift;
	uint8_t pixel;
	int32_t xofs = (patternTable * 0x1000) + tileIndex * 16;

	// greyscale palette reverse-engineered from haiku system palette
	QColor const colors[] = {
		Qt::black,
		Qt::darkGray,
		Qt::lightGray,
		Qt::white,
	};

	Mapper *mapper = nes::cart.mapper();
	if (!mapper) {
		return;
	}

	for (int32_t y = 0; y < 8; y++) {
		uint8_t firstPlane  = mapper->read_vram(xofs + 0);
		uint8_t secondPlane = mapper->read_vram(xofs + 8);
		shift               = 7;

		for (int32_t x = 0; x < 8; x++) {
			pixel = (firstPlane >> shift) & 0x1;
			pixel |= ((secondPlane >> shift) & 0x1) << 1;
			shift--;

			img->setPixelColor(x + (tileX * 8), y + (tileY * 8), colors[pixel]);
		}

		xofs++;
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PatternTableView::on_patternTable0Size_toggled(bool value) {
	Q_UNUSED(value);
	ui_.patternTable0->update();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void PatternTableView::on_patternTable1Size_toggled(bool value) {
	Q_UNUSED(value);
	ui_.patternTable1->update();
}

//------------------------------------------------------------------------------
// Name: eventFilter
//------------------------------------------------------------------------------
bool PatternTableView::eventFilter(QObject *watched, QEvent *event) {

	if (event->type() == QEvent::Paint) {
		if (watched == ui_.patternTable0) {

			QImage back_buffer(128, 128, QImage::Format_RGB32);

			if (ui_.patternTable0Size->isChecked()) {
				drawPatternTable8x16(0, &back_buffer);
			} else {
				drawPatternTable8x8(0, &back_buffer);
			}

			QPainter painter;
			painter.begin(ui_.patternTable0);
			painter.drawImage(ui_.patternTable0->rect(), back_buffer);
			painter.end();

			return true;
		} else {
			if (watched == ui_.patternTable1) {

				QImage back_buffer(128, 128, QImage::Format_RGB32);

				if (ui_.patternTable1Size->isChecked()) {
					drawPatternTable8x16(1, &back_buffer);
				} else {
					drawPatternTable8x8(1, &back_buffer);
				}

				QPainter painter;
				painter.begin(ui_.patternTable1);
				painter.drawImage(ui_.patternTable1->rect(), back_buffer);
				painter.end();
				return true;
			}
		}
	}
	return false;
}
