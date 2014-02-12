#ifndef CPY_20121206_H_
#define CPY_20121206_H_

#include "compare.h"

//------------------------------------------------------------------------------
// Name: opcode_cpy
// Desc: Compare Y Register
//------------------------------------------------------------------------------
struct opcode_cpy : compare {

	void operator()(Context &ctx, uint8_t data) const {
		do_compare(ctx, ctx.Y, data);
	}
};

#endif

