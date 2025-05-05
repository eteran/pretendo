
#ifndef PATTERN_TABLE_VIEW_H
#define PATTERN_TABLE_VIEW_H

#include "ui_PatternTableView.h"
#include <QDialog>

class QImage;

class PatternTableView final : public QDialog {
	Q_OBJECT

public:
	PatternTableView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected Q_SLOTS:
	bool eventFilter(QObject *watched, QEvent *event) override;
	void on_patternTable0Size_toggled(bool value);
	void on_patternTable1Size_toggled(bool value);

public:
	void setupUpdateTimer(QTimer *timer);

private:
	void drawPatternTable8x8(int32_t which, QImage *img);
	void drawPatternTable8x16(int32_t which, QImage *img);
	void drawTile(int32_t patternTable, int32_t tileIndex, int32_t tileX, int32_t tileY, QImage *img);

private:
	Ui::PatternTableView ui_;
};

#endif
