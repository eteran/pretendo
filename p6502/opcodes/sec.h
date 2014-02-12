#ifndef SEC_20121206_H_
#define SEC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sec
// Desc: Set Carry Flag
//------------------------------------------------------------------------------
struct opcode_sec {

	typedef operation_none memory_access;

	void operator()(Context &ctx) const {
		set_flag<C_MASK>(ctx);
	}
};


#endif

