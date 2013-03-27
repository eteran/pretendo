
#ifndef INPUT_20080314_H_
#define INPUT_20080314_H_

#include "Controller.h"

class Input {
public:
	Input();

public:
	void reset();

public:
	void write4016(uint8_t value);
	uint8_t read4016();
	uint8_t read4017();

public:
	Controller &controller1();

private:
	uint8_t    strobe_;
	Controller controller1_;
};

#endif

