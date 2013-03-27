
#ifndef ALSA_AUDIO_20130124_H_
#define ALSA_AUDIO_20130124_H_

#include "Audio.h"
#include <alsa/asoundlib.h>

class AlsaAudio : public Audio {
public:
	AlsaAudio();
	virtual ~AlsaAudio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();

private:
	snd_pcm_t        *handle_;
	snd_pcm_uframes_t buffer_size_;
};

#endif
