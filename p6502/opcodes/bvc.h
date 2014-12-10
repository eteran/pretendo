#ifndef BVC_20121206_H_
#define BVC_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bvc
// Desc: Branch if Overflow Clear
//------------------------------------------------------------------------------
struct opcode_bvc : branch {

	static bool execute() {
		return do_branch(overflow_clear());
	}
};

#endif

