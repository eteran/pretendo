
#include "PulseAudio.h"
#include <iostream>

//------------------------------------------------------------------------------
// Name: PulseAudio
//------------------------------------------------------------------------------
PulseAudio::PulseAudio() {
	std::cout << "[PulseAudio::PulseAudio]" << std::endl;
	
    static const pa_sample_spec ss = { 
		PA_SAMPLE_U8,
		44100,
		1
	};
	

    int error;

    // Create a new playback stream
	if (!(stream_ = pa_simple_new(nullptr, "pretendo", PA_STREAM_PLAYBACK, nullptr, "playback", &ss, nullptr, nullptr, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        abort();
    }

}

//------------------------------------------------------------------------------
// Name: ~PulseAudio
//-------------------------------------------------------------------
PulseAudio::~PulseAudio() {
    if (stream_) {
		pa_simple_free(stream_);
	}
}

//------------------------------------------------------------------------------
// Name: write
//------------------------------------------------------------------------------
void PulseAudio::write(const void *p, size_t n) {
	
	int error;
    if (pa_simple_write(stream_, p, n, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
		abort();		
    }	
}

//------------------------------------------------------------------------------
// Name: start
//------------------------------------------------------------------------------
void PulseAudio::start() {
}

//------------------------------------------------------------------------------
// Name: stop
//------------------------------------------------------------------------------
void PulseAudio::stop() {
}
