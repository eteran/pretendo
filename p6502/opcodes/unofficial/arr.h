#ifndef ARR_20121206_H_
#define ARR_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_arr
// Desc: Logical AND then ROR A with weird flags
//------------------------------------------------------------------------------
struct opcode_arr {

	typedef operation_read memory_access;

	void operator()(uint8_t data) const {
		A &= data;
		
		// only correct because C_MASK is 0x01, we basically want to shift in an 0x80
		// if C_MASK is set, and a 0 if it is not
		const uint8_t bit = (P & C_MASK) << 7;
		A = ((A >> 1) & 0x7f) | bit;
		
		set_flag_condition<C_MASK>(A & 0x40);
		set_flag_condition<V_MASK>(((A & 0x40) >> 1) ^ (A & 0x20));
		update_nz_flags(A);
	}
};

#endif

