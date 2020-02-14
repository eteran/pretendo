#ifndef CMP_20121206_H_
#define CMP_20121206_H_

#include "compare.h"

//------------------------------------------------------------------------------
// Name: opcode_cmp
// Desc: Compare
//------------------------------------------------------------------------------
struct opcode_cmp : compare {

	static void execute(uint8_t data) {
		do_compare(A, data);
	}
};

#endif
