
#ifndef INPUT_20080314_H_
#define INPUT_20080314_H_

#include <cstdint>
#include "NES.h"

using std::uint8_t;

class Controller;

namespace nes {
namespace input {

void reset();

void write4016(uint8_t value);
uint8_t read4016();
uint8_t read4017();

extern Controller controller1;
extern Controller controller2;
extern Controller controller3;
extern Controller controller4;

}
}

#endif

