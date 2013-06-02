
#include "SDLAudio.h"
#include "../APU.h"
#include <iostream>

namespace {

void audio_callback(void * userdata, Uint8* stream, int len) {
	SDL_LockAudio();
	
	SDL_UnlockAudio();
}

}

//------------------------------------------------------------------------------
// Name: SDLAudio
//------------------------------------------------------------------------------
SDLAudio::SDLAudio() {

	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec spec;
	spec.callback = audio_callback;
	spec.userdata = this;
	spec.channels = 1;
	spec.format   = AUDIO_U8;
	spec.freq     = APU::frequency;
	spec.samples  = APU::buffer_size;
	
	SDL_OpenAudio(&spec, 0);
}

//------------------------------------------------------------------------------
// Name: ~SDLAudio
//-------------------------------------------------------------------
SDLAudio::~SDLAudio() {
	SDL_CloseAudio();
}

//------------------------------------------------------------------------------
// Name: write
//------------------------------------------------------------------------------
void SDLAudio::write(const void *p, size_t n) {
	SDL_LockAudio();
	
	SDL_UnlockAudio();

}

//------------------------------------------------------------------------------
// Name: start
//------------------------------------------------------------------------------
void SDLAudio::start() {
	SDL_PauseAudio(0);
}

//------------------------------------------------------------------------------
// Name: stop
//------------------------------------------------------------------------------
void SDLAudio::stop() {
	SDL_PauseAudio(1);
}
