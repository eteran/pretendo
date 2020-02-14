#ifndef ORA_20121206_H_
#define ORA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_ora
// Desc: Logical Inclusive OR
//------------------------------------------------------------------------------
struct opcode_ora {

	using memory_access = operation_read;

	static void execute(uint8_t data) {
		A |= data;
		update_nz_flags(A);
	}
};

#endif
