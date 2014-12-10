#ifndef CPX_20121206_H_
#define CPX_20121206_H_

#include "compare.h"

//------------------------------------------------------------------------------
// Name: opcode_cpx
// Desc: Compare X Register
//------------------------------------------------------------------------------
struct opcode_cpx : compare {

	static void execute(uint8_t data) {
		do_compare(X, data);
	}
};

#endif

