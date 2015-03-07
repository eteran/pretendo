
#include "AlsaAudio.h"
#include "../APU.h"
#include <iostream>

//------------------------------------------------------------------------------
// Name: AlsaAudio
//------------------------------------------------------------------------------
AlsaAudio::AlsaAudio() {

	std::cout << "[AlsaAudio::AlsaAudio]" << std::endl;

	static const int mode = 0; // SND_PCM_NONBLOCK

	if(snd_pcm_open(&handle_, "default", SND_PCM_STREAM_PLAYBACK, mode) < 0) {
		// throw an exception, we failed to even open the device :-(
	}

	snd_pcm_hw_params_t * hwparams;

	unsigned int rate = nes::apu::frequency;

	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_hw_params_any(handle_, hwparams);

	snd_pcm_hw_params_set_access   (handle_, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format   (handle_, hwparams, SND_PCM_FORMAT_U8);
	snd_pcm_hw_params_set_channels (handle_, hwparams, 1);
	snd_pcm_hw_params_set_rate_near(handle_, hwparams, &rate, 0);
	snd_pcm_hw_params(handle_, hwparams);
}

//------------------------------------------------------------------------------
// Name: ~AlsaAudio
//------------------------------------------------------------------------------
AlsaAudio::~AlsaAudio() {
}

//------------------------------------------------------------------------------
// Name: write
//------------------------------------------------------------------------------
void AlsaAudio::write(const void *p, size_t n) {

	if(handle_) {
		int state = snd_pcm_writei(handle_, p, n);
		if(state < 0) {
			snd_pcm_recover(handle_, state, 0);
		}
	}
}

//------------------------------------------------------------------------------
// Name: start
//------------------------------------------------------------------------------
void AlsaAudio::start() {
}

//------------------------------------------------------------------------------
// Name: stop
//------------------------------------------------------------------------------
void AlsaAudio::stop() {
}
