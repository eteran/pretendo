#ifndef SEC_20121206_H_
#define SEC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sec
// Desc: Set Carry Flag
//------------------------------------------------------------------------------
struct opcode_sec {

	using memory_access = operation_none;

	static void execute() {
		set_flag<C_MASK>();
	}
};

#endif
