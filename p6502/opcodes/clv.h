#ifndef CLV_20121206_H_
#define CLV_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_clv
// Desc: Clear Overflow Flag
//------------------------------------------------------------------------------
struct opcode_clv {

	typedef operation_none memory_access;

	static void execute() {
		clear_flag<V_MASK>();
	}
};


#endif

