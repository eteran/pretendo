
#ifndef PULSE_AUDIO_20130124_H_
#define PULSE_AUDIO_20130124_H_

#include <cstddef>
#include <pulse/simple.h>
#include <pulse/error.h>

class PulseAudio final {
public:
	PulseAudio();
	~PulseAudio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();
	
private:
	pa_simple *stream_ = nullptr;
};

#endif
