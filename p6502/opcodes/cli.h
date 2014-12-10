#ifndef CLI_20121206_H_
#define CLI_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_cli
// Desc: Clear Interrupt Disable
//------------------------------------------------------------------------------
struct opcode_cli {

	typedef operation_none memory_access;

	static void execute() {
		clear_flag<I_MASK>();
	}
};


#endif

