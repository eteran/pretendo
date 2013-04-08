
#include "NullAudio.h"
#include <iostream>

//------------------------------------------------------------------------------
// Name: NullAudio
//------------------------------------------------------------------------------
NullAudio::NullAudio() {
	std::cout << "[NullAudio::NullAudio]" << std::endl;
}

//------------------------------------------------------------------------------
// Name: ~NullAudio
//-------------------------------------------------------------------
NullAudio::~NullAudio() {
}

//------------------------------------------------------------------------------
// Name: write
//------------------------------------------------------------------------------
void NullAudio::write(const void *p, size_t n) {
	(void)p;
	(void)n;
}

//------------------------------------------------------------------------------
// Name: start
//------------------------------------------------------------------------------
void NullAudio::start() {
}

//------------------------------------------------------------------------------
// Name: stop
//------------------------------------------------------------------------------
void NullAudio::stop() {
}
