
#include "Pretendo.h"
#include "About.h"
#include "Apu.h"
#include "AudioViewer.h"
#include "Cart.h"
#include "Controller.h"
#include "FilesystemModel.h"
#include "Input.h"
#include "Nes.h"
#include "PatternTableView.h"
#include "Ppu.h"
#include "Preferences.h"
#include "Settings.h"
#include "SortFilterProxyModel.h"

#include <QActionGroup>
#include <QByteArray>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>

#include <algorithm>
#include <iostream>

#if defined(ENABLE_SOUND)
#include "Audio.h"
#else
#include "NullAudio.h"
#endif

namespace {

QString resolve_path(const QString &config_path, const QString &candidate) {
	if (candidate.isEmpty()) {
		return candidate;
	}

	QFileInfo fi(candidate);
	if (fi.isAbsolute()) {
		return fi.absoluteFilePath();
	}

	const QFileInfo config_info(config_path);
	return QDir(config_info.absoluteDir()).absoluteFilePath(candidate);
}

bool files_equal(const QString &lhs_path, const QString &rhs_path) {
	QFile lhs(lhs_path);
	QFile rhs(rhs_path);

	if (!lhs.open(QIODevice::ReadOnly) || !rhs.open(QIODevice::ReadOnly)) {
		return false;
	}

	if (lhs.size() != rhs.size()) {
		return false;
	}

	constexpr qint64 k_chunk_size = 64 * 1024;
	while (!lhs.atEnd()) {
		if (lhs.read(k_chunk_size) != rhs.read(k_chunk_size)) {
			return false;
		}
	}

	return true;
}

}

class EmulationThread final : public QThread {
public:
	EmulationThread(QtVideo *video, QObject *owner)
		: video_(video), owner_(owner) {
	}

	~EmulationThread() override {
		stopLoop();
	}

	void setFrameRate(int framerate) {
		QMutexLocker lock(&state_mutex_);
		frame_rate_ = std::max(1, framerate);
	}

	void startLoop() {
		QMutexLocker lock(&state_mutex_);
		if (running_) {
			paused_ = false;
			state_changed_.wakeAll();
			return;
		}

		running_ = true;
		paused_  = false;
		start();
	}

	void stopLoop() {
		{
			QMutexLocker lock(&state_mutex_);
			running_ = false;
			paused_  = false;
			state_changed_.wakeAll();
		}
		wait();
	}

	void setPaused(bool value) {
		QMutexLocker lock(&state_mutex_);
		paused_ = value;
		if (!paused_) {
			state_changed_.wakeAll();
		}
	}

protected:
	void run() override {
		using Clock = std::chrono::steady_clock;
		auto next_frame_deadline = Clock::now();

		for (;;) {
			{
				QMutexLocker lock(&state_mutex_);
				while (running_ && paused_) {
					state_changed_.wait(&state_mutex_);
				}

				if (!running_) {
					break;
				}
			}

			int frame_rate = 60;
			{
				QMutexLocker lock(&state_mutex_);
				frame_rate = std::max(1, frame_rate_);
			}

			const auto frame_duration = std::chrono::microseconds(1'000'000 / frame_rate);
			next_frame_deadline += frame_duration;

			nes::run_frame(video_);

			uint8_t samples[800];
			size_t count = nes::apu::read_samples(samples, sizeof(samples));
			QByteArray audio_data(reinterpret_cast<const char *>(samples), static_cast<int>(count));

			QMetaObject::invokeMethod(owner_, "onFrameCompleted", Qt::QueuedConnection, Q_ARG(QByteArray, audio_data));

			const auto frame_end = Clock::now();
			if (frame_end > next_frame_deadline) {
				// If we missed the deadline, resync to now to prevent drift accumulation.
				next_frame_deadline = frame_end;
				continue;
			}

			for (;;) {
				const auto now = Clock::now();
				if (now >= next_frame_deadline) {
					break;
				}

				const auto remaining = std::chrono::duration_cast<std::chrono::microseconds>(next_frame_deadline - now);
				if (remaining > std::chrono::microseconds(2000)) {
					QThread::usleep(static_cast<unsigned long>((remaining - std::chrono::microseconds(1000)).count()));
				} else {
					QThread::yieldCurrentThread();
				}
			}
		}
	}

private:
	QtVideo *video_ = nullptr;
	QObject *owner_ = nullptr;
	QMutex state_mutex_;
	QWaitCondition state_changed_;
	bool running_   = false;
	bool paused_    = false;
	int frame_rate_ = 60;
};

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

	// Populate the ROM list
	// NOTE(eteran): might be slow for large lists
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

	filter_model_ = new SortFilterProxyModel(this);
	filter_model_->setFilterCaseSensitivity(Qt::CaseInsensitive);
	filter_model_->setSourceModel(filesystem_model_);
	filter_model_->sort(0, Qt::AscendingOrder);

	ui_.listView->setModel(filter_model_);

	connect(ui_.listView, &QListView::activated, this, &Pretendo::picked);
	connect(ui_.lineEdit, &QLineEdit::textChanged, filter_model_, &QSortFilterProxyModel::setFilterFixedString);

	viewer_timer_ = new QTimer(this);
	viewer_timer_->setInterval(std::max(1, static_cast<int>((1.0f / framerate_) * 1000.0f)));

	emulation_thread_ = new EmulationThread(ui_.video, this);
	emulation_thread_->setFrameRate(framerate_);

#if defined(ENABLE_SOUND)
	audio_ = new Audio();
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

	elapsed_time_ = std::chrono::high_resolution_clock::now();

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
	on_action_Free_ROM_triggered();
	delete emulation_thread_;
	delete audio_;

	Settings::save();
}

//------------------------------------------------------------------------------
// Name: setFrameRate
//------------------------------------------------------------------------------
void Pretendo::setFrameRate(int framerate) {
	framerate_ = std::max(1, framerate);
	viewer_timer_->setInterval(std::max(1, static_cast<int>((1.0f / framerate_) * 1000.0f)));
	emulation_thread_->setFrameRate(framerate_);
}

//------------------------------------------------------------------------------
// Name: setFrameLimit
//------------------------------------------------------------------------------
void Pretendo::setFrameLimit(uint64_t limit) {
	frame_limit_ = limit;
}

//------------------------------------------------------------------------------
// Name: configureRegressionTests
//------------------------------------------------------------------------------
bool Pretendo::configureRegressionTests(const QString &config_path) {
	QFile file(config_path);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "[Pretendo] Failed to open regression config: " << config_path.toStdString() << std::endl;
		return false;
	}

	QJsonParseError parse_error;
	const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parse_error);
	if (parse_error.error != QJsonParseError::NoError || !doc.isObject()) {
		std::cerr << "[Pretendo] Invalid regression JSON: " << parse_error.errorString().toStdString() << std::endl;
		return false;
	}

	const QJsonObject root = doc.object();
	const QString expected_dir = resolve_path(config_path, root.value("expected_dir").toString());
	if (expected_dir.isEmpty()) {
		std::cerr << "[Pretendo] Missing expected_dir in regression config" << std::endl;
		return false;
	}

	const QJsonArray tests = root.value("tests").toArray();
	if (tests.isEmpty()) {
		std::cerr << "[Pretendo] No tests found in regression config" << std::endl;
		return false;
	}

	std::vector<RegressionTest> parsed_tests;
	parsed_tests.reserve(static_cast<size_t>(tests.size()));

	for (qsizetype i = 0; i < tests.size(); ++i) {
		if (!tests[i].isObject()) {
			std::cerr << "[Pretendo] tests[" << i << "] is not an object" << std::endl;
			return false;
		}

		const QJsonObject test = tests[i].toObject();

		bool ok = false;
		const qint64 frame_count_i64 = test.value("frame_count").toVariant().toLongLong(&ok);
		if (!ok || frame_count_i64 <= 0) {
			std::cerr << "[Pretendo] tests[" << i << "] has invalid frame_count" << std::endl;
			return false;
		}

		const QString rom = resolve_path(config_path, test.value("test_rom").toString());
		const QString screenshot = resolve_path(config_path, test.value("screenshot").toString());
		if (rom.isEmpty() || screenshot.isEmpty()) {
			std::cerr << "[Pretendo] tests[" << i << "] is missing test_rom or screenshot" << std::endl;
			return false;
		}

		RegressionTest parsed_test;
		parsed_test.test_rom    = rom;
		parsed_test.screenshot  = screenshot;
		parsed_test.frame_count = static_cast<uint64_t>(frame_count_i64);

		const QJsonArray input_array = test.value("input").toArray();
		for (const QJsonValue &entry : input_array) {
			if (!entry.isObject()) {
				std::cerr << "[Pretendo] tests[" << i << "] input entry is not an object" << std::endl;
				return false;
			}
			const QJsonObject obj = entry.toObject();
			for (auto it = obj.begin(); it != obj.end(); ++it) {
				bool frame_ok            = false;
				const uint64_t frame_num = it.key().toULongLong(&frame_ok);
				if (!frame_ok) {
					std::cerr << "[Pretendo] tests[" << i << "] has invalid input frame number: " << it.key().toStdString() << std::endl;
					return false;
				}
				std::vector<QString> buttons;
				for (const QJsonValue &btn : it.value().toArray()) {
					buttons.push_back(btn.toString());
				}
				parsed_test.input_events[frame_num] = std::move(buttons);
			}
		}

		parsed_tests.push_back(std::move(parsed_test));
	}

	regression_mode_         = true;
	regression_had_failures_ = false;
	regression_current_test_ = 0;
	regression_expected_dir_ = expected_dir;
	regression_tests_        = std::move(parsed_tests);

	std::cout << "[Pretendo] Loaded " << regression_tests_.size() << " regression tests" << std::endl;

	QTimer::singleShot(0, this, [this]() { startNextRegressionTest(); });
	return true;
}

//------------------------------------------------------------------------------
// Name: startNextRegressionTest
//------------------------------------------------------------------------------
void Pretendo::startNextRegressionTest() {
	if (!regression_mode_) {
		return;
	}

	if (regression_current_test_ >= regression_tests_.size()) {
		finishRegressionRun();
		return;
	}

	const RegressionTest &test = regression_tests_[regression_current_test_];

	on_action_Stop_triggered();
	on_action_Free_ROM_triggered();

	std::cout << "[Pretendo] Running test " << (regression_current_test_ + 1) << "/" << regression_tests_.size() << ": "
			  << test.test_rom.toStdString() << " for " << test.frame_count << " frames" << std::endl;

	if (!nes::cart.load(test.test_rom.toStdString())) {
		std::cerr << "[Pretendo] Failed to load ROM: " << test.test_rom.toStdString() << std::endl;
		regression_had_failures_ = true;
		++regression_current_test_;
		QTimer::singleShot(0, this, [this]() { startNextRegressionTest(); });
		return;
	}

	setFrameLimit(test.frame_count);
	on_action_Run_triggered();
}

//------------------------------------------------------------------------------
// Name: finishRegressionRun
//------------------------------------------------------------------------------
void Pretendo::finishRegressionRun() {
	on_action_Stop_triggered();
	on_action_Free_ROM_triggered();

	const int exit_code = regression_had_failures_ ? 1 : 0;
	if (exit_code == 0) {
		std::cout << "[Pretendo] Regression run complete: all compared screenshots matched" << std::endl;
	} else {
		std::cout << "[Pretendo] Regression run complete: mismatches detected" << std::endl;
	}

	QTimer::singleShot(0, this, [this, exit_code]() {
		close();
		QCoreApplication::exit(exit_code);
	});
}

//------------------------------------------------------------------------------
// Name: saveScreenshot
//------------------------------------------------------------------------------
bool Pretendo::saveScreenshot(const QString &path) {
	if (!nes::cart.mapper()) {
		return false;
	}

	const QFileInfo screenshot_info(path);
	QDir().mkpath(screenshot_info.absolutePath());

	QImage screenshot = ui_.video->screenshot();
	return screenshot.save(path);
}

//------------------------------------------------------------------------------
// Name: onFrameCompleted
//------------------------------------------------------------------------------
void Pretendo::onFrameCompleted(const QByteArray &samples) {
	if (!running_ || paused_) {
		return;
	}

	ui_.video->end_frame();

	audio_->write(samples.constData(), static_cast<size_t>(samples.size()));

	// FPS calculation
	auto now      = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - elapsed_time_);
	if (duration.count() > 1000) {
		fps_label_->setText(tr("FPS: %1").arg(framecount_));
		elapsed_time_ = now;
		framecount_   = 0;
	}

	++framecount_;

	++raw_framecount_;

	// Apply scheduled controller inputs for regression tests
	if (regression_mode_ && regression_current_test_ < regression_tests_.size()) {
		const RegressionTest &cur_test = regression_tests_[regression_current_test_];

		// Clear all automation inputs each frame so buttons don't stay pressed
		nes::input::controller1.keystate_.reset();

		const auto it = cur_test.input_events.find(raw_framecount_);
		if (it != cur_test.input_events.end()) {
			bool do_reset = false;
			for (const QString &btn : it->second) {
				if (btn.compare(QLatin1String("RESET"), Qt::CaseInsensitive) == 0) {
					do_reset = true;
				} else if (btn.compare(QLatin1String("A"), Qt::CaseInsensitive) == 0) {
					nes::input::controller1.keystate_[Controller::INDEX_A] = true;
				} else if (btn.compare(QLatin1String("B"), Qt::CaseInsensitive) == 0) {
					nes::input::controller1.keystate_[Controller::INDEX_B] = true;
				} else if (btn.compare(QLatin1String("SELECT"), Qt::CaseInsensitive) == 0) {
					nes::input::controller1.keystate_[Controller::INDEX_SELECT] = true;
				} else if (btn.compare(QLatin1String("START"), Qt::CaseInsensitive) == 0) {
					nes::input::controller1.keystate_[Controller::INDEX_START] = true;
				} else if (btn.compare(QLatin1String("UP"), Qt::CaseInsensitive) == 0) {
					nes::input::controller1.keystate_[Controller::INDEX_UP] = true;
				} else if (btn.compare(QLatin1String("DOWN"), Qt::CaseInsensitive) == 0) {
					nes::input::controller1.keystate_[Controller::INDEX_DOWN] = true;
				} else if (btn.compare(QLatin1String("LEFT"), Qt::CaseInsensitive) == 0) {
					nes::input::controller1.keystate_[Controller::INDEX_LEFT] = true;
				} else if (btn.compare(QLatin1String("RIGHT"), Qt::CaseInsensitive) == 0) {
					nes::input::controller1.keystate_[Controller::INDEX_RIGHT] = true;
				} else {
					std::cerr << "[Pretendo] Unknown input button: " << btn.toStdString() << std::endl;
				}
			}

			if (do_reset) {
				emulation_thread_->stopLoop();
				viewer_timer_->stop();
				audio_->stop();
				nes::reset(nes::Reset::Soft);
				emulation_thread_->startLoop();
				viewer_timer_->start();
				audio_->start();
				return;
			}
		}
	}

	if (frame_limit_ != 0 && raw_framecount_ >= frame_limit_) {
		if (regression_mode_ && regression_current_test_ < regression_tests_.size()) {
			const RegressionTest test = regression_tests_[regression_current_test_];
			if (!saveScreenshot(test.screenshot)) {
				std::cerr << "[Pretendo] Failed to save screenshot: " << test.screenshot.toStdString() << std::endl;
				regression_had_failures_ = true;
			} else {
				const QString expected = QDir(regression_expected_dir_).filePath(QFileInfo(test.screenshot).fileName());
				if (QFileInfo::exists(expected)) {
					const bool matches = files_equal(test.screenshot, expected);
					if (!matches) {
						std::cerr << "[Pretendo] MISMATCH: " << test.screenshot.toStdString() << " != " << expected.toStdString() << std::endl;
						regression_had_failures_ = true;
					} else {
						std::cout << "[Pretendo] MATCH: " << QFileInfo(test.screenshot).fileName().toStdString() << std::endl;
					}
				} else {
					std::cout << "[Pretendo] SKIP (no expected image): " << expected.toStdString() << std::endl;
				}
			}

			on_action_Stop_triggered();
			on_action_Free_ROM_triggered();
			setFrameLimit(0);

			++regression_current_test_;
			QTimer::singleShot(0, this, [this]() { startNextRegressionTest(); });
		} else {
			on_action_Take_Screenshot_triggered();
			on_action_Stop_triggered();
			on_action_Free_ROM_triggered();
			qApp->quit();
		}
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

		if (!running_) {

			// we test mapper, it's a good metric for "did we load the cart correctly"
			if (nes::cart.mapper()) {

				ui_.stackedWidget->setCurrentIndex(1);

				nes::reset(nes::Reset::Hard);

				raw_framecount_ = 0;
				framecount_     = 0;
				elapsed_time_   = std::chrono::high_resolution_clock::now();

				emulation_thread_->startLoop();
				viewer_timer_->start();
				audio_->start();
				running_ = true;
				paused_  = false;

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
	emulation_thread_->stopLoop();
	viewer_timer_->stop();
	audio_->stop();
	running_        = false;
	paused_         = false;
	framecount_     = 0;
	raw_framecount_ = 0;
	ui_.stackedWidget->setCurrentIndex(0);
	fps_label_->setText(tr("FPS: 0"));
}

//------------------------------------------------------------------------------
// Name: on_action_Pause_triggered
//------------------------------------------------------------------------------
void Pretendo::on_action_Pause_triggered() {
	if (!running_) {
		return;
	}

	if (!paused_) {
		emulation_thread_->setPaused(true);
		viewer_timer_->stop();
		audio_->stop();
		paused_ = true;
	} else {
		if (nes::cart.mapper()) {
			emulation_thread_->setPaused(false);
			viewer_timer_->start();
			audio_->start();
			paused_ = false;
		}
	}
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
	if (!running_) {
		return;
	}

	if (paused_) {
		nes::reset(nes::Reset::Hard);
		return;
	}

	emulation_thread_->stopLoop();
	viewer_timer_->stop();
	audio_->stop();

	nes::reset(nes::Reset::Hard);

	emulation_thread_->startLoop();
	viewer_timer_->start();
	audio_->start();
}

//------------------------------------------------------------------------------
// Name: on_actionReset_triggered
//------------------------------------------------------------------------------
void Pretendo::on_actionReset_triggered() {
	if (!running_) {
		return;
	}

	if (paused_) {
		nes::reset(nes::Reset::Soft);
		return;
	}

	emulation_thread_->stopLoop();
	viewer_timer_->stop();
	audio_->stop();

	nes::reset(nes::Reset::Soft);

	emulation_thread_->startLoop();
	viewer_timer_->start();
	audio_->start();
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
	const bool was_running = running_ && !paused_;

	if (was_running) {
		on_action_Pause_triggered();
	}

	preferences_->exec();

	if (was_running) {
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
		dialog->setupUpdateTimer(viewer_timer_);
	}
	dialog->show();
}

//------------------------------------------------------------------------------
// Name:
// Desc:
//------------------------------------------------------------------------------
void Pretendo::on_action_Pattern_Table_Viewer_triggered() {
	static PatternTableView *dialog = nullptr;
	if (!dialog) {
		dialog = new PatternTableView(this);
		dialog->setupUpdateTimer(viewer_timer_);
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

	QString filename = QString::fromStdString(nes::cart.filename());

	QFileInfo fi(filename);
	const QString output = QString("pretendo-%1-%2.png").arg(nes::cart.rom_hash(), 8, 16, QLatin1Char('0')).arg(fi.baseName());
	if (!saveScreenshot(output)) {
		QMessageBox::critical(this, tr("Error Taking Snapshot"), tr("Failed to write screenshot"));
	}
}
