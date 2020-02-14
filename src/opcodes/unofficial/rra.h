#ifndef RRA_20121206_H_
#define RRA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_rra
// Desc: Rotate Memory Right and then add to A
//------------------------------------------------------------------------------
struct opcode_rra {

	using memory_access = operation_modify;

	static void execute(uint8_t &data) {
		// only correct because C_MASK is 0x01, we basically want to shift in an 0x80
		// if C_MASK is set, and a 0 if it is not
		const uint8_t bit = (P & C_MASK) << 7;
		set_flag_condition<C_MASK>(data & 0x01);
		data = ((data >> 1) & 0x7f) | bit;

		const uint16_t temp16 = A + data + (P & C_MASK);
		const bool carry      = (temp16 > 0xff);
		const bool overflow   = ((~(A ^ data)) & (A ^ temp16) & 0x80) != 0;
		set_flag_condition<V_MASK>(overflow);
		set_flag_condition<C_MASK>(carry);
		A = temp16 & 0x00ff;
		update_nz_flags(A);
	}
};

#endif
