#ifndef CPY_20121206_H_
#define CPY_20121206_H_

#include "compare.h"

//------------------------------------------------------------------------------
// Name: opcode_cpy
// Desc: Compare Y Register
//------------------------------------------------------------------------------
struct opcode_cpy : compare {

	static void execute(uint8_t data) {
		do_compare(Y, data);
	}
};

#endif

