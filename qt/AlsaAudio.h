
#ifndef ALSA_AUDIO_20130124_H_
#define ALSA_AUDIO_20130124_H_

#include "APU.h"
#include <alsa/asoundlib.h>
#include <cstdint>

class AlsaAudio {
public:
	AlsaAudio();
	~AlsaAudio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();

private:
	snd_pcm_t           *handle_;
	snd_pcm_hw_params_t *hwparams_;
	snd_pcm_sw_params_t	*swparams_;
};

#endif
