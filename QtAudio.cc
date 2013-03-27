
#include "QtAudio.h"
#include "AudioBuffer.h"
#include <QtMultimedia/QAudioOutput>
#include <iostream>

//------------------------------------------------------------------------------
// Name: QtAudio
//------------------------------------------------------------------------------
QtAudio::QtAudio(QObject *parent) : QObject(parent) {

	std::cout << "[QtAudio::QtAudio]" << std::endl;

	QAudioFormat format;

	// Set up the format
	format.setChannels(1);
	format.setFrequency(44100);
	format.setSampleSize(8);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::UnSignedInt);

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(format)) {
		qWarning() << "raw audio format not supported by backend, cannot play audio.";
	}

	audio_ = new QAudioOutput(format, this);

	audio_->setBufferSize(735);

	audio_buffer_ = new AudioBuffer(this);
}

//------------------------------------------------------------------------------
// Name: ~QtAudio
//-------------------------------------------------------------------
QtAudio::~QtAudio() {
}

//------------------------------------------------------------------------------
// Name: write
//------------------------------------------------------------------------------
void QtAudio::write(const void *p, size_t n) {
	audio_buffer_->writeData(reinterpret_cast<const char *>(p), n);
}

//------------------------------------------------------------------------------
// Name: start
//------------------------------------------------------------------------------
void QtAudio::start() {
	audio_->start(audio_buffer_);
}

//------------------------------------------------------------------------------
// Name: stop
//------------------------------------------------------------------------------
void QtAudio::stop() {
	audio_->stop();
}
