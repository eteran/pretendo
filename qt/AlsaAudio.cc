
#include "AlsaAudio.h"
#include <iostream>

//------------------------------------------------------------------------------
// Name: AlsaAudio
//------------------------------------------------------------------------------
AlsaAudio::AlsaAudio() {

	std::cout << "[AlsaAudio::AlsaAudio]" << std::endl;

	snd_pcm_open(&handle_, "default", SND_PCM_STREAM_PLAYBACK, /*SND_PCM_NONBLOCK | */0);

	snd_pcm_hw_params_t * hwparams;
	snd_pcm_sw_params_t * swparams;
	snd_pcm_status_t *    status;

	const unsigned int pchannels  = 1;
	unsigned int pspeed           = 44100;
	const snd_pcm_format_t format = SND_PCM_FORMAT_U8;
	unsigned int buffer_time      = 4000;
	unsigned int period_time      = buffer_time / 4;

	snd_pcm_sw_params_alloca(&swparams);

	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_hw_params_any(handle_, hwparams);

	snd_pcm_hw_params_set_access          (handle_, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format          (handle_, hwparams, format);
	snd_pcm_hw_params_set_channels        (handle_, hwparams, pchannels);
	snd_pcm_hw_params_set_rate_near       (handle_, hwparams, &pspeed, 0);
	snd_pcm_hw_params_set_buffer_time_near(handle_, hwparams, &buffer_time, 0);
	snd_pcm_hw_params_set_period_time_near(handle_, hwparams, &period_time, 0);
	snd_pcm_hw_params(handle_, hwparams);

	snd_pcm_status_alloca(&status);
	snd_pcm_status(handle_, status);
	buffer_size_ = snd_pcm_status_get_avail(status);
}

//------------------------------------------------------------------------------
// Name: ~AlsaAudio
//-------------------------------------------------------------------
AlsaAudio::~AlsaAudio() {
}

//------------------------------------------------------------------------------
// Name: write
//------------------------------------------------------------------------------
void AlsaAudio::write(const void *p, size_t n) {
	if(handle_) {
		if(snd_pcm_state(handle_) == SND_PCM_STATE_XRUN) {
			snd_pcm_prepare(handle_);
		}
		snd_pcm_writei(handle_, p, n);
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
