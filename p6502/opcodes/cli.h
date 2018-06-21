#ifndef CLI_20121206_H_
#define CLI_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_cli
// Desc: Clear Interrupt Disable
//------------------------------------------------------------------------------
struct opcode_cli {

	using memory_access = operation_none;

	static void execute() {
		clear_flag<I_MASK>();
	}
};


#endif

