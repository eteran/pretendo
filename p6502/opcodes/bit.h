
#ifndef BIT_20121206_H_
#define BIT_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_bit
// Desc: Bit Test
//------------------------------------------------------------------------------
struct opcode_bit {

	using memory_access = operation_read;

	static void execute(uint8_t data) {
		set_flag_condition<Z_MASK>(!(data & A));
		set_flag_condition<N_MASK>(data & N_MASK);
		set_flag_condition<V_MASK>(data & V_MASK);
	}
};

#endif

