#ifndef BEQ_20121206_H_
#define BEQ_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_beq
// Desc: Branch if Equal
//------------------------------------------------------------------------------
struct opcode_beq : branch {

	bool operator()() const {
		return do_branch(zero_set());
	}
};

#endif

