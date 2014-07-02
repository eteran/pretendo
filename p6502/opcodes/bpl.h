#ifndef BPL_20121206_H_
#define BPL_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bpl
// Desc: Branch if Positive
//------------------------------------------------------------------------------
struct opcode_bpl : branch {

	bool operator()() const {
		return do_branch(negative_clear());
	}
};

#endif

