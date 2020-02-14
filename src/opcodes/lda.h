#ifndef LDA_20121206_H_
#define LDA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_lda
// Desc: Load Accumulator
//------------------------------------------------------------------------------
struct opcode_lda {

	using memory_access = operation_read;

	static void execute(uint8_t data) {
		A = data;
		update_nz_flags(A);
	}
};

#endif
