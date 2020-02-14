#ifndef CLC_20121206_H_
#define CLC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_clc
// Desc: Clear Carry Flag
//------------------------------------------------------------------------------
struct opcode_clc {

	using memory_access = operation_none;

	static void execute() {
		clear_flag<C_MASK>();
	}
};

#endif
