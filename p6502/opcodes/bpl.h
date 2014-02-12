#ifndef BPL_20121206_H_
#define BPL_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bpl
// Desc: Branch if Positive
//------------------------------------------------------------------------------
struct opcode_bpl : branch {

	bool operator()(Context &ctx) const {
		return do_branch(ctx, negative_clear());
	}
};

#endif

