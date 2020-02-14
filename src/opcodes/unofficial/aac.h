#ifndef AAC_20121206_H_
#define AAC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_aac
// Desc: Logical AND then copies N to C
//------------------------------------------------------------------------------
struct opcode_aac {

	using memory_access = operation_read;

	static void execute(uint8_t data) {
		A &= data;
		update_nz_flags(A);
		P = (P & ~C_MASK) | ((P & N_MASK) >> 7);
	}
};

#endif
