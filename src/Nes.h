
#ifndef NES_H_
#define NES_H_

#include "Reset.h"

class VideoInterface;
class Cart;
class Config;

namespace nes {

extern Cart   cart;
extern Config config;

void run_frame(VideoInterface *video);
void reset(Reset reset_type);

}

#endif
