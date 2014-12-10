#ifndef RLA_20121206_H_
#define RLA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_rla
// Desc: Rotate Left
//------------------------------------------------------------------------------
struct opcode_rla {

	typedef operation_modify memory_access;
	
	static void execute(uint8_t &data) {
		// only correct because C_MASK is 0x01, we basically want to shift in a 1
		// if C_MASK is set, and a 0 if it is not
		const uint8_t bit = P & C_MASK;
		set_flag_condition<C_MASK>(data & 0x80);
		data = (data << 1) | bit;
		A &= data;
		update_nz_flags(A);
	}
};

#endif

