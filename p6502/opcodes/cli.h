#ifndef CLI_20121206_H_
#define CLI_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_cli
// Desc: Clear Interrupt Disable
//------------------------------------------------------------------------------
struct opcode_cli {

	typedef operation_none memory_access;

	void operator()(Context &ctx) const {
		clear_flag<I_MASK>(ctx);
	}
};


#endif

