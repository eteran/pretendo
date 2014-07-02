#ifndef CMP_20121206_H_
#define CMP_20121206_H_

#include "compare.h"

//------------------------------------------------------------------------------
// Name: opcode_cmp
// Desc: Compare
//------------------------------------------------------------------------------
struct opcode_cmp : compare {

	void operator()(uint8_t data) const {
		do_compare(A, data);
	}
};

#endif

