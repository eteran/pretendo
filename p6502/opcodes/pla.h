#ifndef PLA_20121206_H_
#define PLA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_pla
// Desc: Pull Accumulator
//------------------------------------------------------------------------------
struct opcode_pla {

    using memory_access = operation_stack_read;

	static void execute(uint8_t data) {
		A = data;
		update_nz_flags(A);
	}
};

#endif

