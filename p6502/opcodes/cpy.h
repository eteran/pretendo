#ifndef CPY_20121206_H_
#define CPY_20121206_H_

#include "compare.h"

//------------------------------------------------------------------------------
// Name: opcode_cpy
// Desc: Compare Y Register
//------------------------------------------------------------------------------
struct opcode_cpy : compare {

	void operator()(uint8_t data) const {
		do_compare(Y, data);
	}
};

#endif

