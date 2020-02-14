#ifndef TAX_20121206_H_
#define TAX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tax
// Desc: Transfer A to X
//------------------------------------------------------------------------------
struct opcode_tax {
	using memory_access = operation_none;

	static void execute() {
		X = A;
		update_nz_flags(X);
	}
};

#endif
