#ifndef BNE_20121206_H_
#define BNE_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bne
// Desc: Branch if Not Equal
//------------------------------------------------------------------------------
struct opcode_bne : branch {

	bool operator()(Context &ctx) const {
		return do_branch(ctx, zero_clear());
	}
};


#endif

