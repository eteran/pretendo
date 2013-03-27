#ifndef BNE_20121206_H_
#define BNE_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bne
// Desc: Branch if Not Equal
//------------------------------------------------------------------------------
struct opcode_bne : branch {

	bool operator()() const {
		return do_branch(zero_clear());
	}
};


#endif

