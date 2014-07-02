#ifndef CLC_20121206_H_
#define CLC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_clc
// Desc: Clear Carry Flag
//------------------------------------------------------------------------------
struct opcode_clc {

	typedef operation_none memory_access;

	void operator()() const {
		clear_flag<C_MASK>();
	}
};


#endif

