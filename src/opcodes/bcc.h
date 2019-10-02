#ifndef BCC_20121206_H_
#define BCC_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bcc
// Desc: Branch if Carry Clear
//------------------------------------------------------------------------------
struct opcode_bcc : branch {

	static bool execute() {
		return do_branch(carry_clear());
	}
};

#endif

