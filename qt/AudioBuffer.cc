
#include "AudioBuffer.h"
#include <QtDebug>
#include <QMutexLocker>

AudioBuffer::AudioBuffer() : QIODevice(), read_pos_(0), write_pos_(0) {
	qFill(buffer_, buffer_ +  sizeof(buffer_), 0x80);
	open(QIODevice::ReadWrite);
}

AudioBuffer::AudioBuffer(QObject *parent) : QIODevice(parent), read_pos_(0), write_pos_(0) {
	qFill(buffer_, buffer_ +  sizeof(buffer_), 0x80);
	open(QIODevice::ReadWrite);
}

qint64 AudioBuffer::readData(char *data, qint64 maxSize) {


	QMutexLocker lock(&mutex_);

	qint64 i;
	for(i = 0; i < maxSize; ++i) {
		// allow a NULL data dest
		if(data) {
			data[i] = buffer_[read_pos_++ % sizeof(buffer_)];
		} else {
			read_pos_++;
		}
		if(read_pos_ == write_pos_) {
			break;
		}
	}

	return i;
}

qint64 AudioBuffer::writeData(const char *data, qint64 maxSize) {

	QMutexLocker lock(&mutex_);

	for(qint64 i = 0; i < maxSize; ++i) {
		buffer_[write_pos_++ % sizeof(buffer_)] = data[i];
	}

	return maxSize;
}

bool AudioBuffer::atEnd() const {
	return false;
}

qint64 AudioBuffer::bytesAvailable() const {
	QMutexLocker lock(&mutex_);
	return write_pos_ - read_pos_;
}

bool AudioBuffer::isSequential() const {
	return true;
}

qint64 AudioBuffer::size() const {
	return bytesAvailable();
}
