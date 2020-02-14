#ifndef EOR_20121206_H_
#define EOR_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_eor
// Desc: Exclusive OR
//------------------------------------------------------------------------------
struct opcode_eor {

	using memory_access = operation_read;

	static void execute(uint8_t data) {
		A ^= data;
		update_nz_flags(A);
	}
};

#endif
