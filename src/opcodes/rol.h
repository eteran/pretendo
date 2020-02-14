#ifndef ROL_20121206_H_
#define ROL_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_rol
// Desc: Rotate Left
//------------------------------------------------------------------------------
struct opcode_rol {

	using memory_access = operation_modify;

	static void execute(uint8_t &data) {
		// only correct because C_MASK is 0x01, we basically want to shift in a 1
		// if C_MASK is set, and a 0 if it is not
		const uint8_t bit = P & C_MASK;
		set_flag_condition<C_MASK>(data & 0x80);
		data = (data << 1) | bit;
		update_nz_flags(data);
	}
};

#endif
