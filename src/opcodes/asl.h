
#ifndef ASL_20121206_H_
#define ASL_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_asl
// Desc: Arithmetic Shift Left
//------------------------------------------------------------------------------
struct opcode_asl {

    using memory_access = operation_modify;
	
	static void execute(uint8_t &data) {
		set_flag_condition<C_MASK>(data & 0x80);
		data <<= 1;
		update_nz_flags(data);
	}
};

#endif

