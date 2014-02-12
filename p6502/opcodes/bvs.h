#ifndef BVS_20121206_H_
#define BVS_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bvs
// Desc: Branch if Overflow Set
//------------------------------------------------------------------------------
struct opcode_bvs : branch {

	bool operator()(Context &ctx) const {
		return do_branch(ctx, overflow_set());
	}
};

#endif

