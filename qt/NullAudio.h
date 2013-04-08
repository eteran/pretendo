
#ifndef NULL_AUDIO_20130124_H_
#define NULL_AUDIO_20130124_H_

#include <cstddef>

class NullAudio {
public:
	NullAudio();
	~NullAudio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();
};

#endif
