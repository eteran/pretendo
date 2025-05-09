#ifndef AUDIO_H
#define AUDIO_H

class QIODevice;
class QAudioSink;

class Audio final {
public:
	Audio();
	~Audio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();

private:
	QIODevice *outputDevice;
	QAudioSink *audioSink;
};

#endif // AUDIO_H
