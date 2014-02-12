#ifndef CMP_20121206_H_
#define CMP_20121206_H_

#include "compare.h"

//------------------------------------------------------------------------------
// Name: opcode_cmp
// Desc: Compare
//------------------------------------------------------------------------------
struct opcode_cmp : compare {

	void operator()(Context &ctx, uint8_t data) const {
		do_compare(ctx, ctx.A, data);
	}
};

#endif

