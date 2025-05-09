
#include "Audio.h"
#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <iostream>

//------------------------------------------------------------------------------
// Name: Audio
//------------------------------------------------------------------------------
Audio::Audio() {

	QAudioFormat format;
	format.setSampleRate(48000);
	format.setChannelCount(1);
	format.setSampleFormat(QAudioFormat::SampleFormat::UInt8);

	audioSink = new QAudioSink(format);

	std::cout << "[Audio::Audio]" << std::endl;
}

//------------------------------------------------------------------------------
// Name: Audio
//------------------------------------------------------------------------------
Audio::~Audio() {
	delete audioSink;
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
	outputDevice = audioSink->start();
}

//------------------------------------------------------------------------------
// Name: stop
//------------------------------------------------------------------------------
void Audio::stop() {
	audioSink->stop();
}
