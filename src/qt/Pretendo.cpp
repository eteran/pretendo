
#include "Pretendo.h"
#include "About.h"
#include "AudioViewer.h"
#include "Cart.h"
#include "Controller.h"
#include "FilesystemModel.h"
#include "Input.h"
#include "Mapper.h"
#include "Nes.h"
#include "Ppu.h"
#include "Preferences.h"
#include "Settings.h"
#include "SortFilterProxyModel.h"

#include <QDateTime>
#include <QDirIterator>
#include <QFileDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QThread>
#include <QTimer>

#include <iostream>

#if defined(PULSE_AUDIO_SOUND)
#include "PulseAudio.h"
#else
#include "NullAudio.h"
#endif

//------------------------------------------------------------------------------
// Name: Pretendo
//------------------------------------------------------------------------------
Pretendo::Pretendo(const QString &filename, QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags) {
	ui_.setupUi(this);

	Settings::load();

	ui_.actionShow_Sprites->setChecked(Settings::showSprites);

	// make only one of these selectable at a time
	QActionGroup *const zoom_group = new QActionGroup(this);
	zoom_group->addAction(ui_.action1x);
	zoom_group->addAction(ui_.action2x);
	zoom_group->addAction(ui_.action3x);
	zoom_group->addAction(ui_.action4x);

	switch (Settings::zoomFactor) {
	case 1:
		ui_.action1x->setChecked(true);
		break;
	case 2:
		ui_.action2x->setChecked(true);
		break;
	case 3:
		ui_.action3x->setChecked(true);
		break;
	case 4:
		ui_.action4x->setChecked(true);
		break;
	}

	// set the default zoom
	zoom(Settings::zoomFactor);

	preferences_ = new Preferences(this);

	fps_label_ = new QLabel(tr("FPS: 0"));

	ui_.toolBar->addSeparator();
	ui_.toolBar->addWidget(fps_label_);

	QStringList filters;
	filters << "*.nes"
			<< "*.nes.gz";

	filesystem_model_ = new FilesystemModel(this);

	auto thread = QThread::create([this]() {
		auto romdir = QString::fromStdString(Settings::romDirectory);
		QFileInfo romdir_fi(romdir);
		romdir_fi.makeAbsolute();

		QString rom_basedir = romdir_fi.path();
		if (!rom_basedir.endsWith('/')) {
			rom_basedir.append('/');
		}

		QDirIterator it(romdir, QStringList() << "*.nes", QDir::Files | QDir::Readable, QDirIterator::Subdirectories);
		while (it.hasNext()) {
			QString f = it.next();
			QFileInfo fi(f);
			fi.makeAbsolute();

			QString path = fi.filePath();
			if (path.startsWith(rom_basedir)) {
				path = path.mid(rom_basedir.size());
			}

			filesystem_model_->addFile(FilesystemModel::Item{path, f});
		}
	});

	thread->start();

	filter_model_ = new SortFilterProxyModel(this);
	filter_model_->setFilterCaseSensitivity(Qt::CaseInsensitive);
	filter_model_->setSourceModel(filesystem_model_);
	filter_model_->sort(0, Qt::AscendingOrder);

	ui_.listView->setModel(filter_model_);

	connect(ui_.listView, &QListView::activated, this, &Pretendo::picked);
	connect(ui_.lineEdit, &QLineEdit::textChanged, filter_model_, &QSortFilterProxyModel::setFilterFixedString);

	timer_ = new QTimer(this);
	connect(timer_, &QTimer::timeout, this, &Pretendo::update);

#if defined(PULSE_AUDIO_SOUND)
	audio_ = new PulseAudio();
#else
	audio_ = new NullAudio();
#endif

	// setup default palette
	ui_.video->set_palette(Palette::intensity, Palette::NTSC(
												   Palette::default_saturation,
												   Palette::default_hue,
												   Palette::default_contrast,
												   Palette::default_brightness,
												   Palette::default_gamma));

	time_.start();

	// set the default player 1 controlls
	player_1_[Controller::INDEX_A]      = Qt::Key_X;
	player_1_[Controller::INDEX_B]      = Qt::Key_Z;
	player_1_[Controller::INDEX_SELECT] = Qt::Key_A;
	player_1_[Controller::INDEX_START]  = Qt::Key_S;
	player_1_[Controller::INDEX_UP]     = Qt::Key_Up;
	player_1_[Controller::INDEX_DOWN]   = Qt::Key_Down;
	player_1_[Controller::INDEX_LEFT]   = Qt::Key_Left;
	player_1_[Controller::INDEX_RIGHT]  = Qt::Key_Right;

	if (!filename.isNull()) {
		const QString rom = filename;

		// make the ROM viewer default to the location of the run ROM
		const QFileInfo info(rom);
		if (info.isFile()) {
			if (!nes::cart.load(rom.toStdString())) {
				QTimer::singleShot(0, this, SLOT(close()));
				std::cout << "[Pretendo] Failed to load ROM, exiting..." << std::endl;
				return;
			}
			on_action_Run_triggered();
		}
	}
}

//------------------------------------------------------------------------------
// Name: ~Pretendo
//------------------------------------------------------------------------------
Pretendo::~Pretendo() {
	on_action_Stop_triggered();
	on_action_Free_ROM_triggered();
	delete audio_;

	Settings::save();
}

//------------------------------------------------------------------------------
// Name: setFrameRate
//------------------------------------------------------------------------------
void Pretendo::setFrameRate(int framerate) {
	framerate_ = framerate;
	if (timer_->isActive()) {
		timer_->start(1000.0f / framerate_);
	}
}

//------------------------------------------------------------------------------
// Name: setFrameLimit
//------------------------------------------------------------------------------
void Pretendo::setFrameLimit(uint64_t limit) {
	frame_limit_ = limit;
}

//------------------------------------------------------------------------------
// Name: update
//------------------------------------------------------------------------------
void Pretendo::update() {

	// idle processing loop (the emulation loop)
	nes::run_frame(ui_.video);
	ui_.video->end_frame();

	const size_t audio_head     = nes::apu::sample_buffer_.head();
	const size_t audio_tail     = nes::apu::sample_buffer_.tail();
	const size_t audio_capacity = nes::apu::sample_buffer_.capacity();
	const auto audio_buffer     = nes::apu::sample_buffer_.buffer();

	if (audio_head >= audio_tail) {
		audio_->write(&audio_buffer[audio_head], audio_capacity - audio_head);
		audio_->write(&audio_buffer[0], audio_tail);
	} else {
		audio_->write(&audio_buffer[audio_head], audio_tail - audio_head);
	}
	nes::apu::sample_buffer_.clear();

	// FPS calculation
	if (time_.elapsed() > 1000) {
		fps_label_->setText(tr("FPS: %1").arg(framecount_));
		time_.restart();
		framecount_ = 0;
	}

	++framecount_;

	++raw_framecount_;
	if (raw_framecount_ == frame_limit_) {
		on_action_Take_Screenshot_triggered();
		on_action_Stop_triggered();
		on_action_Free_ROM_triggered();
		qApp->quit();
	}
}

//------------------------------------------------------------------------------
// Name: on_action_Load_ROM_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Load_ROM_triggered() {

	const QString rom = QFileDialog::getOpenFileName(this, tr("Open ROM File"), QString(), tr("iNES ROM Images (*.nes *.nes.gz)"));
	if (!rom.isNull()) {
		on_action_Stop_triggered();
		on_action_Free_ROM_triggered();
		nes::cart.load(rom.toStdString());
	}
}

//------------------------------------------------------------------------------
// Name: on_action_Free_ROM_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Free_ROM_triggered() {
	on_action_Stop_triggered();
	nes::cart.unload();
}

//------------------------------------------------------------------------------
// Name: picked
//------------------------------------------------------------------------------
void Pretendo::picked(const QModelIndex &index) {
	if (index.isValid()) {
		if (const QAbstractItemModel *const m = index.model()) {
			if (const QSortFilterProxyModel *const filter_model = qobject_cast<const QSortFilterProxyModel *>(m)) {
				if (FilesystemModel *const fs_model = qobject_cast<FilesystemModel *>(filter_model->sourceModel())) {

					const QModelIndex source_index = filter_model->mapToSource(index);
					const auto filename            = fs_model->data(source_index, Qt::UserRole).toString();

					// they picked a ROM, load it, then run it
					if (!filename.isEmpty()) {
						on_action_Stop_triggered();
						on_action_Free_ROM_triggered();
						nes::cart.load(filename.toStdString());
						on_action_Run_triggered();
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name: on_action_Run_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Run_triggered() {

	if (paused_) {
		on_action_Pause_triggered();
	} else {

		if (!timer_->isActive()) {

			// we test mapper, it's a good metric for "did we load the cart correctly"
			if (nes::cart.mapper()) {

				ui_.stackedWidget->setCurrentIndex(1);

				nes::reset(nes::Reset::Hard);

				raw_framecount_ = 0;

				timer_->start(1000.0f / framerate_);
				audio_->start();
				paused_ = false;

				ui_.action_Pause->setEnabled(true);
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name: on_action_Stop_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Stop_triggered() {
	ui_.action_Pause->setEnabled(false);
	timer_->stop();
	audio_->stop();
	paused_ = false;
	ui_.stackedWidget->setCurrentIndex(0);
	fps_label_->setText(tr("FPS: 0"));
}

//------------------------------------------------------------------------------
// Name: on_action_Pause_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Pause_triggered() {

	if (timer_->isActive()) {
		timer_->stop();
		audio_->stop();
	} else if (paused_) {
		if (nes::cart.mapper()) {
			timer_->start();
			audio_->start();
		}
	}

	paused_ = !timer_->isActive();
}

//------------------------------------------------------------------------------
// Name: keyPressEvent
//------------------------------------------------------------------------------
void Pretendo::keyPressEvent(QKeyEvent *event) {

	if (event->isAutoRepeat()) {
		return;
	}

	const int key = event->key();

	if (key == player_1_[Controller::INDEX_A]) {
		nes::input::controller1.keystate_[Controller::INDEX_A] = true;
	} else if (key == player_1_[Controller::INDEX_B]) {
		nes::input::controller1.keystate_[Controller::INDEX_B] = true;
	} else if (key == player_1_[Controller::INDEX_SELECT]) {
		nes::input::controller1.keystate_[Controller::INDEX_SELECT] = true;
	} else if (key == player_1_[Controller::INDEX_START]) {
		nes::input::controller1.keystate_[Controller::INDEX_START] = true;
	} else if (key == player_1_[Controller::INDEX_UP]) {
		nes::input::controller1.keystate_[Controller::INDEX_UP] = true;
	} else if (key == player_1_[Controller::INDEX_DOWN]) {
		nes::input::controller1.keystate_[Controller::INDEX_DOWN] = true;
	} else if (key == player_1_[Controller::INDEX_LEFT]) {
		nes::input::controller1.keystate_[Controller::INDEX_LEFT] = true;
	} else if (key == player_1_[Controller::INDEX_RIGHT]) {
		nes::input::controller1.keystate_[Controller::INDEX_RIGHT] = true;
	} else {
		event->ignore();
	}
}

//------------------------------------------------------------------------------
// Name: keyReleaseEvent
//------------------------------------------------------------------------------
void Pretendo::keyReleaseEvent(QKeyEvent *event) {

	const int key = event->key();

	if (event->isAutoRepeat()) {
		return;
	}

	if (key == player_1_[Controller::INDEX_A]) {
		nes::input::controller1.keystate_[Controller::INDEX_A] = false;
	} else if (key == player_1_[Controller::INDEX_B]) {
		nes::input::controller1.keystate_[Controller::INDEX_B] = false;
	} else if (key == player_1_[Controller::INDEX_SELECT]) {
		nes::input::controller1.keystate_[Controller::INDEX_SELECT] = false;
	} else if (key == player_1_[Controller::INDEX_START]) {
		nes::input::controller1.keystate_[Controller::INDEX_START] = false;
	} else if (key == player_1_[Controller::INDEX_UP]) {
		nes::input::controller1.keystate_[Controller::INDEX_UP] = false;
	} else if (key == player_1_[Controller::INDEX_DOWN]) {
		nes::input::controller1.keystate_[Controller::INDEX_DOWN] = false;
	} else if (key == player_1_[Controller::INDEX_LEFT]) {
		nes::input::controller1.keystate_[Controller::INDEX_LEFT] = false;
	} else if (key == player_1_[Controller::INDEX_RIGHT]) {
		nes::input::controller1.keystate_[Controller::INDEX_RIGHT] = false;
	} else {
		event->ignore();
	}
}

//------------------------------------------------------------------------------
// Name: on_actionShow_Sprites_toggled
//------------------------------------------------------------------------------
void Pretendo::on_actionShow_Sprites_toggled(bool value) {
	nes::ppu::show_sprites = value;
	Settings::showSprites  = value;
}

//------------------------------------------------------------------------------
// Name: on_action_Hard_Reset_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Hard_Reset_triggered() {

	if (paused_) {
		on_action_Pause_triggered();
	}

	if (timer_->isActive()) {
		nes::reset(nes::Reset::Hard);
	}
}

//------------------------------------------------------------------------------
// Name: on_actionReset_triggered
//------------------------------------------------------------------------------
void Pretendo::on_actionReset_triggered() {
	if (paused_) {
		on_action_Pause_triggered();
	}

	if (timer_->isActive()) {
		nes::reset(nes::Reset::Soft);
	}
}

//------------------------------------------------------------------------------
// Name: zoom
//------------------------------------------------------------------------------
void Pretendo::zoom(int scale) {
	ui_.video->setFixedSize(256 * scale, 240 * scale);

	QWidget *w = ui_.video->parentWidget();
	while (w) {
		w->adjustSize();
		w = w->parentWidget();
	}
}

//------------------------------------------------------------------------------
// Name: on_action1x_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action1x_triggered() {
	zoom(1);
	Settings::zoomFactor = 1;
}

//------------------------------------------------------------------------------
// Name: on_action2x_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action2x_triggered() {
	zoom(2);
	Settings::zoomFactor = 2;
}

//------------------------------------------------------------------------------
// Name: on_action3x_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action3x_triggered() {
	zoom(3);
	Settings::zoomFactor = 3;
}

//------------------------------------------------------------------------------
// Name: on_action4x_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action4x_triggered() {
	zoom(4);
	Settings::zoomFactor = 4;
}

//------------------------------------------------------------------------------
// Name: on_action_Preferences_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Preferences_triggered() {

	if (timer_->isActive()) {
		timer_->stop();
		audio_->stop();
	}
	paused_ = !timer_->isActive();

	preferences_->exec();

	if (paused_) {
		on_action_Pause_triggered();
	}
}

//------------------------------------------------------------------------------
// Name: on_actionAbout_Qt_triggered
// Desc: shows an About Qt dialog box
//------------------------------------------------------------------------------
void Pretendo::on_actionAbout_Qt_triggered() {
	QMessageBox::aboutQt(this, tr("About Qt"));
}

//------------------------------------------------------------------------------
// Name: on_action_About_triggered
// Desc: shows an About dialog box
//------------------------------------------------------------------------------
void Pretendo::on_action_About_triggered() {
	static About *dialog = nullptr;
	if (!dialog) {
		dialog = new About(this);
		dialog->ui_.build_date->setText(tr("%1").arg(__TIMESTAMP__));
		dialog->ui_.version->setText(tr("%1").arg("2.0.0"));
	}
	dialog->show();
}

//------------------------------------------------------------------------------
// Name:
// Desc:
//------------------------------------------------------------------------------
void Pretendo::on_action_Audio_Viewer_triggered() {
	static AudioViewer *dialog = nullptr;
	if (!dialog) {
		dialog = new AudioViewer(this);
		connect(timer_, &QTimer::timeout, dialog, &AudioViewer::update);
	}
	dialog->show();
}

//------------------------------------------------------------------------------
// Name:
// Desc:
//------------------------------------------------------------------------------
void Pretendo::on_action_Take_Screenshot_triggered() {

	if (!nes::cart.mapper()) {
		QMessageBox::critical(this, tr("Error Taking Snapshot"), tr("Cannot take a screenshot with no ROM loaded"));
		return;
	}

	QImage screenshot = ui_.video->screenshot();
	auto pix          = QPixmap::fromImage(screenshot);

	QString filename = QString::fromStdString(nes::cart.filename());

	QFileInfo fi(filename);

	pix.save(QString("pretendo-%1-%2.png").arg(nes::cart.rom_hash(), 8, 16, QLatin1Char('0')).arg(fi.baseName()));
}
