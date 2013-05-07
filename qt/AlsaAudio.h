
#ifndef ALSA_AUDIO_20130124_H_
#define ALSA_AUDIO_20130124_H_

#include <alsa/asoundlib.h>

class AlsaAudio {
public:
	AlsaAudio();
	~AlsaAudio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();

private:
	snd_pcm_t *handle_;
};

#endif
