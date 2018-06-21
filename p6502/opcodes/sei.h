#ifndef SEI_20121206_H_
#define SEI_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sei
// Desc: Set Interrupt Disable
//------------------------------------------------------------------------------
struct opcode_sei {

	using memory_access = operation_none;

	static void execute() {
		set_flag<I_MASK>();
	}
};


#endif

