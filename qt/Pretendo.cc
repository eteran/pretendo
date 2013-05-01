
#include "Pretendo.h"
#include "Controller.h"
#include "Mapper.h"
#include "NES.h"
#include "SortFilterProxyModel.h"
#include <QDebug>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QTimer>
#include <QLabel>

#if defined(ENABLE_SOUND) && defined(USE_QAUDIO)
#include "QtAudio.h"
#elif defined(ENABLE_SOUND)
#include "AlsaAudio.h"
#else
#include "NullAudio.h"
#endif

namespace {

//const int timer_interval = 1000. / 60;
const int timer_interval = 0;

}

//------------------------------------------------------------------------------
// Name: Pretendo
//------------------------------------------------------------------------------
Pretendo::Pretendo(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags), timer_(0), fps_label_(0), framecount_(0), paused_(false) {
	ui_.setupUi(this);

	fps_label_ = new QLabel(tr("FPS: 0"));

	ui_.toolBar->addSeparator();
	ui_.toolBar->addWidget(fps_label_);

	QStringList filters;
	filters << "*.nes";

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
	connect(timer_, SIGNAL(timeout()), this, SLOT(update()));

#if defined(ENABLE_SOUND) && defined(USE_QAUDIO)
	audio_ = new QtAudio(this);
#elif defined(ENABLE_SOUND)
	audio_ = new AlsaAudio();
#else
	audio_ = new NullAudio();
#endif

	time_.start();
}

//------------------------------------------------------------------------------
// Name: ~Pretendo
//------------------------------------------------------------------------------
Pretendo::~Pretendo() {
	on_action_Stop_triggered();
	on_action_Free_ROM_triggered();
}

//------------------------------------------------------------------------------
// Name: update
//------------------------------------------------------------------------------
void Pretendo::update() {

	// idle processing loop (the emulation loop)

	ui_.video->start_frame();
	nes::run_frame(ui_.video);
	ui_.video->end_frame();

#if 0
	const APU::buffer b = nes::apu.fill_buffer();
	audio_->write(b.ptr, b.length);
#endif

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

	const QString rom = QFileDialog::getOpenFileName(this, tr("Open ROM File"), QString(), tr("iNES ROM Images (*.nes)"));
	if(!rom.isNull()) {
		on_action_Stop_triggered();
		on_action_Free_ROM_triggered();
		nes::cart.load(qPrintable(rom));
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
							nes::cart.load(qPrintable(filename));
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
			if(const boost::shared_ptr<Mapper> mapper = nes::cart.mapper()) {

				ui_.stackedWidget->setCurrentIndex(1);

				// setup video driver stuff
				ui_.video->set_palette(Palette::intensity, Palette::NTSCPalette(355.00, 0.50));

				nes::reset(nes::HARD_RESET);

				timer_->start(timer_interval);
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
		if(const boost::shared_ptr<Mapper> mapper = nes::cart.mapper()) {
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
	switch(event->key()) {
	case Qt::Key_X:
		nes::input.controller1().keystate_[Controller::INDEX_A]      = true;
		break;
	case Qt::Key_Z:
		nes::input.controller1().keystate_[Controller::INDEX_B]      = true;
		break;
	case Qt::Key_A:
		nes::input.controller1().keystate_[Controller::INDEX_SELECT] = true;
		break;
	case Qt::Key_S:
		nes::input.controller1().keystate_[Controller::INDEX_START]  = true;
		break;
	case Qt::Key_Up:
		nes::input.controller1().keystate_[Controller::INDEX_UP]     = true;
		break;
	case Qt::Key_Down:
		nes::input.controller1().keystate_[Controller::INDEX_DOWN]   = true;
		break;
	case Qt::Key_Left:
		nes::input.controller1().keystate_[Controller::INDEX_LEFT]   = true;
		break;
	case Qt::Key_Right:
		nes::input.controller1().keystate_[Controller::INDEX_RIGHT]  = true;
		break;
	default:
		event->ignore();
	}
}

//------------------------------------------------------------------------------
// Name: keyReleaseEvent
//------------------------------------------------------------------------------
void Pretendo::keyReleaseEvent(QKeyEvent *event) {
	switch(event->key()) {
	case Qt::Key_X:
		nes::input.controller1().keystate_[Controller::INDEX_A]      = false;
		break;
	case Qt::Key_Z:
		nes::input.controller1().keystate_[Controller::INDEX_B]      = false;
		break;
	case Qt::Key_A:
		nes::input.controller1().keystate_[Controller::INDEX_SELECT] = false;
		break;
	case Qt::Key_S:
		nes::input.controller1().keystate_[Controller::INDEX_START]  = false;
		break;
	case Qt::Key_Up:
		nes::input.controller1().keystate_[Controller::INDEX_UP]     = false;
		break;
	case Qt::Key_Down:
		nes::input.controller1().keystate_[Controller::INDEX_DOWN]   = false;
		break;
	case Qt::Key_Left:
		nes::input.controller1().keystate_[Controller::INDEX_LEFT]   = false;
		break;
	case Qt::Key_Right:
		nes::input.controller1().keystate_[Controller::INDEX_RIGHT]  = false;
		break;
	default:
		event->ignore();
	}
}

//------------------------------------------------------------------------------
// Name: on_actionShow_Sprites_toggled
//------------------------------------------------------------------------------
void Pretendo::on_actionShow_Sprites_toggled(bool value) {
	nes::ppu.show_sprites_ = value;
}

//------------------------------------------------------------------------------
// Name: on_action_Hard_Reset_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Hard_Reset_triggered() {

	if(paused_) {
		on_action_Pause_triggered();
	}

	if(timer_->isActive()) {
		nes::reset(nes::HARD_RESET);
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
		nes::reset(nes::SOFT_RESET);
	}
}

//------------------------------------------------------------------------------
// Name: showEvent
//------------------------------------------------------------------------------
void Pretendo::showEvent(QShowEvent *event) {
	Q_UNUSED(event);

	if(qApp->arguments().size() == 2) {
		const QString rom = qApp->arguments()[1];

		// make the ROM viewer default to the location of the run ROM
		const QFileInfo info(rom);
		const QModelIndex root_model_index = filesystem_model_->setRootPath(info.absolutePath());
		ui_.listView->setRootIndex(filter_model_->mapFromSource(root_model_index));

		if(info.isFile()) {
			nes::cart.load(qPrintable(rom));
			on_action_Run_triggered();
		}
	}
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Pretendo::on_action1x_triggered() {
	ui_.video->setFixedSize(256 * 1, 240 * 1);
	//ui_.stackedWidget->setFixedSize(256 * 1, 240 * 1);
	adjustSize();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Pretendo::on_action2x_triggered() {
	ui_.video->setFixedSize(256 * 2, 240 * 2);
	//ui_.stackedWidget->setFixedSize(256 * 2, 240 * 2);
	adjustSize();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Pretendo::on_action3x_triggered() {
	ui_.video->setFixedSize(256 * 3, 240 * 3);
	//ui_.stackedWidget->setFixedSize(256 * 3, 240 * 3);
	adjustSize();
}

//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
void Pretendo::on_action4x_triggered() {
	ui_.video->setFixedSize(256 * 4, 240 * 4);
	//ui_.stackedWidget->setFixedSize(256 * 4, 240 * 4);
	adjustSize();
}
