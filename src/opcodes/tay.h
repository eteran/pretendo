#ifndef TAY_20121206_H_
#define TAY_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tay
// Desc: Transfer A to Y
//------------------------------------------------------------------------------
struct opcode_tay {
	using memory_access = operation_none;

	static void execute() {
		Y = A;
		update_nz_flags(Y);
	}
};

#endif
