#ifndef AUDIO_H
#define AUDIO_H

class QIODevice;
class QAudioSink;

#include <QAudio>
#include <QObject>
#include <cstddef>

class Audio final : public QObject {
public:
	explicit Audio(QObject *parent = nullptr);
	~Audio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();

private:
	void handleStateChanged(QAudio::State newState);

private:
	QIODevice *outputDevice = nullptr;
	QAudioSink *audioSink   = nullptr;
};

#endif // AUDIO_H
