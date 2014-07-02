
#ifndef AUDIO_BUFFER_20121217_H_
#define AUDIO_BUFFER_20121217_H_

#include "APU.h"
#include <QIODevice>
#include <QMutex>

class AudioBuffer : public QIODevice {
	Q_OBJECT
public:
	AudioBuffer();
	AudioBuffer(QObject *parent = 0);

public:
	virtual bool atEnd() const;
	virtual bool isSequential() const;
	virtual qint64 bytesAvailable() const;
	virtual qint64 readData(char *data, qint64 maxSize);
	virtual qint64 size() const;
	virtual qint64 writeData(const char *data, qint64 maxSize);

private:
	mutable QMutex mutex_;
	quint64 read_pos_;
	quint64 write_pos_;
	char buffer_[nes::apu::buffer_size * 30];
};

#endif
