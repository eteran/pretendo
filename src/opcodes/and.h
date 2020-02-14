#ifndef AND_20121206_H_
#define AND_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_and
// Desc: Logical AND
//------------------------------------------------------------------------------
struct opcode_and {

	using memory_access = operation_read;

	static void execute(uint8_t data) {
		A &= data;
		update_nz_flags(A);
	}
};

#endif
