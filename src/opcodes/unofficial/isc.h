#ifndef ISC_20121206_H_
#define ISC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_isc
// Desc: Increment Memory & CMP
//------------------------------------------------------------------------------
struct opcode_isc : compare {

	using memory_access = operation_modify;

	static void execute(uint8_t &data) {
		++data;
		const uint16_t temp16 = A - data - ((P & C_MASK) ^ C_MASK);
		const bool carry      = (temp16 < 0x100);
		const bool overflow   = ((A ^ data) & (A ^ temp16) & 0x80) != 0;
		set_flag_condition<V_MASK>(overflow);
		set_flag_condition<C_MASK>(carry);
		A = (temp16 & 0x00ff);
		update_nz_flags(A);
	}
};

#endif
