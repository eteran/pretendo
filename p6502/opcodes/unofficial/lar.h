#ifndef LAR_20121206_H_
#define LAR_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_lar
// Desc: AND memory with S, transfer result to A, X and S.
//------------------------------------------------------------------------------
struct opcode_lar {

	using memory_access = operation_read;
	
	static void execute(uint8_t data) {

		data &= S;
		A = data;
		X = data;
		S = data;
		update_nz_flags(A);
	}
};

#endif

