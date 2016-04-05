
#ifndef QT_AUDIO_20130124_H_
#define QT_AUDIO_20130124_H_

#include <QObject>

class AudioBuffer;
class QAudioOutput;

class QtAudio : public QObject {
	Q_OBJECT

public:
	explicit QtAudio(QObject *parent = 0);
	virtual ~QtAudio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();

private:
	QAudioOutput *audio_;
	AudioBuffer  *audio_buffer_;
};

#endif
