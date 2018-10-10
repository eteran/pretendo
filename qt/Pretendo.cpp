
#include "Pretendo.h"
#include "About.h"
#include "AudioViewer.h"
#include "Cart.h"
#include "Controller.h"
#include "Input.h"
#include "Mapper.h"
#include "NES.h"
#include "PPU.h"
#include "Preferences.h"
#include "SortFilterProxyModel.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QTimer>
#include <QLabel>
#include <QMessageBox>

#if defined(PULSE_AUDIO_SOUND)
#include "PulseAudio.h"
#else
#include "NullAudio.h"
#endif

//------------------------------------------------------------------------------
// Name: Pretendo
//------------------------------------------------------------------------------
Pretendo::Pretendo(const QString &filename, QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags) {
	ui_.setupUi(this);
	
	// make only one of these selectable at a time
	QActionGroup *const zoom_group = new QActionGroup(this);
	zoom_group->addAction(ui_.action1x);
	zoom_group->addAction(ui_.action2x);
	zoom_group->addAction(ui_.action3x);
	zoom_group->addAction(ui_.action4x);
	ui_.action2x->setChecked(true);

	// set the default zoom
	zoom(2);

	preferences_ = new Preferences(this);

	fps_label_ = new QLabel(tr("FPS: 0"));

	ui_.toolBar->addSeparator();
	ui_.toolBar->addWidget(fps_label_);

	QStringList filters;
	filters << "*.nes" << "*.nes.gz";

	filesystem_model_ = new QFileSystemModel(this);
	filesystem_model_->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);
	filesystem_model_->setNameFilters(filters);
	filesystem_model_->setNameFilterDisables(false);
	filesystem_model_->setResolveSymlinks(true);
	const QModelIndex root_model_index = filesystem_model_->setRootPath(QDir::currentPath());

	filter_model_ = new SortFilterProxyModel(this);
	filter_model_->setFilterCaseSensitivity(Qt::CaseInsensitive);
	filter_model_->setSourceModel(filesystem_model_);

	ui_.listView->setModel(filter_model_);
	ui_.listView->setRootIndex(filter_model_->mapFromSource(root_model_index));

	connect(ui_.listView, SIGNAL(activated(const QModelIndex &)), this, SLOT(picked(const QModelIndex &)));
	connect(ui_.lineEdit, SIGNAL(textChanged(const QString &)), filter_model_, SLOT(setFilterFixedString(const QString &)));

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
	
	if(!filename.isNull()) {
		const QString rom = filename;

		// make the ROM viewer default to the location of the run ROM
		const QFileInfo info(rom);
		const QModelIndex root_model_index = filesystem_model_->setRootPath(info.absolutePath());
		ui_.listView->setRootIndex(filter_model_->mapFromSource(root_model_index));

		if(info.isFile()) {
			nes::cart.load(rom.toStdString());
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
}

//------------------------------------------------------------------------------
// Name: setFrameRate
//------------------------------------------------------------------------------
void Pretendo::setFrameRate(int framerate) {
	framerate_ = framerate;
	if(timer_->isActive()) {
		timer_->start(1000. / framerate_);
	}
}

//------------------------------------------------------------------------------
// Name: update
//------------------------------------------------------------------------------
void Pretendo::update() {

	// idle processing loop (the emulation loop)
	nes::run_frame(ui_.video);
	ui_.video->end_frame();

	// get a copy of the audio buffer...
    size_t i      = 0;
    size_t total = audio_buffer_.size();
    while(!nes::apu::sample_buffer_.empty() && i < total) {
        audio_buffer_[i++] = nes::apu::sample_buffer_.front();
        nes::apu::sample_buffer_.pop_front();
    }
	
    audio_->write(&audio_buffer_[0], i);

	// FPS calculation
	if(time_.elapsed() > 1000) {
		fps_label_->setText(tr("FPS: %1").arg(framecount_));
		time_.restart();
		framecount_ = 0;
	}
	++framecount_;

}

//------------------------------------------------------------------------------
// Name: on_action_Load_ROM_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Load_ROM_triggered() {

	const QString rom = QFileDialog::getOpenFileName(this, tr("Open ROM File"), QString(), tr("iNES ROM Images (*.nes *.nes.gz)"));
	if(!rom.isNull()) {
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
	if(index.isValid()) {
		if(const QAbstractItemModel *const m = index.model()) {
			if(const QSortFilterProxyModel *const filter_model = qobject_cast<const QSortFilterProxyModel *>(m)) {
				if(QFileSystemModel *const fs_model = qobject_cast<QFileSystemModel *>(filter_model->sourceModel())) {

					const QModelIndex source_index     = filter_model->mapToSource(index);
					const QString filename             = fs_model->filePath(source_index);
					const QModelIndex root_model_index = fs_model->setRootPath(filename);

					if(fs_model->isDir(source_index)) {
						ui_.listView->setRootIndex(filter_model_->mapFromSource(root_model_index));
					} else {
						// they picked a ROM, load it, then run it
						if(!filename.isEmpty()) {
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
}

//------------------------------------------------------------------------------
// Name: on_action_Run_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Run_triggered() {

	if(paused_) {
		on_action_Pause_triggered();
	} else {

		if(!timer_->isActive()) {

			// we test mapper, it's a good metric for "did we load the cart correctly"
			if(const std::shared_ptr<Mapper> &mapper = nes::cart.mapper()) {

				ui_.stackedWidget->setCurrentIndex(1);

				nes::reset(nes::Reset::Hard);

				timer_->start(1000. / framerate_);
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

	if(timer_->isActive()) {
		timer_->stop();
		audio_->stop();
	} else if(paused_) {
		if(const std::shared_ptr<Mapper> mapper = nes::cart.mapper()) {
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

	if(event->isAutoRepeat()) {
		return;
	}

	const int key = event->key();

	if(key == player_1_[Controller::INDEX_A]) {
		nes::input::controller1.keystate_[Controller::INDEX_A] = true;
	} else if(key == player_1_[Controller::INDEX_B]) {
		nes::input::controller1.keystate_[Controller::INDEX_B] = true;
	} else if(key == player_1_[Controller::INDEX_SELECT]) {
		nes::input::controller1.keystate_[Controller::INDEX_SELECT] = true;
	} else if(key == player_1_[Controller::INDEX_START]) {
		nes::input::controller1.keystate_[Controller::INDEX_START] = true;
	} else if(key == player_1_[Controller::INDEX_UP]) {
		nes::input::controller1.keystate_[Controller::INDEX_UP] = true;
	} else if(key == player_1_[Controller::INDEX_DOWN]) {
		nes::input::controller1.keystate_[Controller::INDEX_DOWN] = true;
	} else if(key == player_1_[Controller::INDEX_LEFT]) {
		nes::input::controller1.keystate_[Controller::INDEX_LEFT] = true;
	} else if(key == player_1_[Controller::INDEX_RIGHT]) {
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

	if(event->isAutoRepeat()) {
		return;
	}

	if(key == player_1_[Controller::INDEX_A]) {
		nes::input::controller1.keystate_[Controller::INDEX_A] = false;
	} else if(key == player_1_[Controller::INDEX_B]) {
		nes::input::controller1.keystate_[Controller::INDEX_B] = false;
	} else if(key == player_1_[Controller::INDEX_SELECT]) {
		nes::input::controller1.keystate_[Controller::INDEX_SELECT] = false;
	} else if(key == player_1_[Controller::INDEX_START]) {
		nes::input::controller1.keystate_[Controller::INDEX_START] = false;
	} else if(key == player_1_[Controller::INDEX_UP]) {
		nes::input::controller1.keystate_[Controller::INDEX_UP] = false;
	} else if(key == player_1_[Controller::INDEX_DOWN]) {
		nes::input::controller1.keystate_[Controller::INDEX_DOWN] = false;
	} else if(key == player_1_[Controller::INDEX_LEFT]) {
		nes::input::controller1.keystate_[Controller::INDEX_LEFT] = false;
	} else if(key == player_1_[Controller::INDEX_RIGHT]) {
		nes::input::controller1.keystate_[Controller::INDEX_RIGHT] = false;
	} else {
		event->ignore();
	}
}

//------------------------------------------------------------------------------
// Name: on_actionShow_Sprites_toggled
//------------------------------------------------------------------------------
void Pretendo::on_actionShow_Sprites_toggled(bool value) {
	nes::ppu::show_sprites_ = value;
}

//------------------------------------------------------------------------------
// Name: on_action_Hard_Reset_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Hard_Reset_triggered() {

	if(paused_) {
		on_action_Pause_triggered();
	}

	if(timer_->isActive()) {
		nes::reset(nes::Reset::Hard);
	}
}

//------------------------------------------------------------------------------
// Name: on_actionReset_triggered
//------------------------------------------------------------------------------
void Pretendo::on_actionReset_triggered() {
	if(paused_) {
		on_action_Pause_triggered();
	}

	if(timer_->isActive()) {
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
}

//------------------------------------------------------------------------------
// Name: on_action2x_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action2x_triggered() {
	zoom(2);
}

//------------------------------------------------------------------------------
// Name: on_action3x_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action3x_triggered() {
	zoom(3);
}

//------------------------------------------------------------------------------
// Name: on_action4x_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action4x_triggered() {
	zoom(4);
}

//------------------------------------------------------------------------------
// Name: on_action_Preferences_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Preferences_triggered() {

	if(timer_->isActive()) {
		timer_->stop();
		audio_->stop();
	}
	paused_ = !timer_->isActive();

	preferences_->exec();

	if(paused_) {
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
	if(!dialog) {
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
	if(!dialog) {
		dialog = new AudioViewer(this);
		connect(timer_, SIGNAL(timeout()), dialog, SLOT(update()));
	}
	dialog->show();
}
