#ifndef CLD_20121206_H_
#define CLD_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_cld
// Desc: Clear Decimal Mode
//------------------------------------------------------------------------------
struct opcode_cld {

	typedef operation_none memory_access;

	void operator()(Context &ctx) const {
		clear_flag<D_MASK>(ctx);
	}
};


#endif

