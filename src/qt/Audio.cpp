
#include "Audio.h"
#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <QtDebug>
#include <iostream>

//------------------------------------------------------------------------------
// Name: Audio
//------------------------------------------------------------------------------
Audio::Audio(QObject *parent)
	: QObject(parent) {

	QAudioFormat format;
	format.setSampleRate(48000);
	format.setChannelCount(1);
	format.setSampleFormat(QAudioFormat::SampleFormat::UInt8);

	audioSink = new QAudioSink(format);
	// audioSink->setBufferSize(800);
	connect(audioSink, &QAudioSink::stateChanged, this, &Audio::handleStateChanged);

	qDebug() << audioSink->bufferSize();

	std::cout << "[Audio::Audio]" << std::endl;
}

//------------------------------------------------------------------------------
// Name: Audio
//------------------------------------------------------------------------------
Audio::~Audio() {
	delete audioSink;
}

//------------------------------------------------------------------------------
// Name: handleStateChanged
//------------------------------------------------------------------------------
void Audio::handleStateChanged(QAudio::State newState) {
	switch (newState) {
	case QtAudio::ActiveState:
		// qDebug() << "Audio Active";
		break;
	case QtAudio::SuspendedState:
		// qDebug() << "Audio Suspended";
		break;
	case QtAudio::StoppedState:
		// qDebug() << "Audio Stopped";
		break;
	case QtAudio::IdleState:
		// qDebug() << "Audio Idle";
		break;
	}
}

//------------------------------------------------------------------------------
// Name: write
//------------------------------------------------------------------------------
void Audio::write(const void *p, size_t n) {
	if (outputDevice) {
		outputDevice->write(static_cast<const char *>(p), n);
	}
}

//------------------------------------------------------------------------------
// Name: start
//------------------------------------------------------------------------------
void Audio::start() {
	// starting a sink that is already running would strand the previous
	// QIODevice, which the sink owns and we would never hear from again
	if (audioSink->state() == QtAudio::StoppedState) {
		outputDevice = audioSink->start();
	}
}

//------------------------------------------------------------------------------
// Name: stop
//------------------------------------------------------------------------------
void Audio::stop() {
	// the sink owns outputDevice and invalidates it here
	audioSink->stop();
	outputDevice = nullptr;
}
