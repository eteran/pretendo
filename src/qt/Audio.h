#ifndef AUDIO_H
#define AUDIO_H

class QIODevice;
class QAudioSink;

#include <QAudio>
#include <QObject>

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
	QIODevice *outputDevice;
	QAudioSink *audioSink;
};

#endif // AUDIO_H
