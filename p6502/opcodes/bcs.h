#ifndef BCS_20121206_H_
#define BCS_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bcs
// Desc: Branch if Carry Set
//------------------------------------------------------------------------------
struct opcode_bcs : branch {

	static bool execute() {
		return do_branch(carry_set());
	}
};

#endif

