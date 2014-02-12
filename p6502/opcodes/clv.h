#ifndef CLV_20121206_H_
#define CLV_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_clv
// Desc: Clear Overflow Flag
//------------------------------------------------------------------------------
struct opcode_clv {

	typedef operation_none memory_access;

	void operator()(Context &ctx) const {
		clear_flag<V_MASK>(ctx);
	}
};


#endif

