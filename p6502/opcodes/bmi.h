#ifndef BMI_20121206_H_
#define BMI_20121206_H_

#include "branch.h"

//------------------------------------------------------------------------------
// Name: opcode_bmi
// Desc: Branch if Minus
//------------------------------------------------------------------------------
struct opcode_bmi : branch {

	bool operator()() const {
		return do_branch(negative_set());
	}
};

#endif

