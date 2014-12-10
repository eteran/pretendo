#ifndef ORA_20121206_H_
#define ORA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_ora
// Desc: Logical Inclusive OR
//------------------------------------------------------------------------------
struct opcode_ora {

	typedef operation_read memory_access;

	static void execute(uint8_t data) {
		A |= data;
		update_nz_flags(A);
	}
};

#endif

