#ifndef SED_20121206_H_
#define SED_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_sed
// Desc: Set Decimal Flag
//------------------------------------------------------------------------------
struct opcode_sed {

	typedef operation_none memory_access;

	static void execute() {
		set_flag<D_MASK>();
	}
};



#endif

