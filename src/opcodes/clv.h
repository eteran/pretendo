#ifndef CLV_20121206_H_
#define CLV_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_clv
// Desc: Clear Overflow Flag
//------------------------------------------------------------------------------
struct opcode_clv {

	using memory_access = operation_none;

	static void execute() {
		clear_flag<V_MASK>();
	}
};

#endif
