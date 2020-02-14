#ifndef CLD_20121206_H_
#define CLD_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_cld
// Desc: Clear Decimal Mode
//------------------------------------------------------------------------------
struct opcode_cld {

	using memory_access = operation_none;

	static void execute() {
		clear_flag<D_MASK>();
	}
};

#endif
