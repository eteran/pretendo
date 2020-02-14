#ifndef BPL_20121206_H_
#define BPL_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bpl
// Desc: Branch if Positive
//------------------------------------------------------------------------------
struct opcode_bpl : branch {

	static bool execute() {
		return do_branch(negative_clear());
	}
};

#endif
