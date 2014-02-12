#ifndef SEI_20121206_H_
#define SEI_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sei
// Desc: Set Interrupt Disable
//------------------------------------------------------------------------------
struct opcode_sei {

	typedef operation_none memory_access;

	void operator()(Context &ctx) const {
		set_flag<I_MASK>(ctx);
	}
};


#endif

