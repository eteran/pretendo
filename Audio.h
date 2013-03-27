
#ifndef AUDIO_20130124_H_
#define AUDIO_20130124_H_

#include <cstddef>

class Audio {
public:
	virtual ~Audio() {}

public:
	virtual void write(const void *p, size_t n) = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
};

#endif
