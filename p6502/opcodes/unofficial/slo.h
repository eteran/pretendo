#ifndef SLO_20121206_H_
#define SLO_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_slo
// Desc: Shift left one bit in memory, then OR accumulator with memory.
//------------------------------------------------------------------------------
struct opcode_slo {

	using memory_access = operation_modify;
	
	static void execute(uint8_t &data) {
		set_flag_condition<C_MASK>(data & 0x80);
		data <<= 1;
		A |= data;
		update_nz_flags(A);
	}
};

#endif

