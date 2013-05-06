
#ifndef SDL_AUDIO_20130124_H_
#define SDL_AUDIO_20130124_H_

#include <SDL/SDL.h>

class SDLAudio {
public:
	SDLAudio();
	~SDLAudio();

public:
	void write(const void *p, size_t n);
	void start();
	void stop();

private:
};

#endif
