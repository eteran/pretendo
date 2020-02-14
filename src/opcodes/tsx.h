#ifndef TSX_20121206_H_
#define TSX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_tsx
// Desc: Transfer S to X
//------------------------------------------------------------------------------
struct opcode_tsx {
	using memory_access = operation_none;

	static void execute() {
		X = S;
		update_nz_flags(X);
	}
};

#endif
