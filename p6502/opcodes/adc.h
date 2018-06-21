#ifndef ADC_20121206_H_
#define ADC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_adc
// Desc: Add with Carry
//------------------------------------------------------------------------------
struct opcode_adc {

    using memory_access = operation_read;

	static void execute(uint8_t data) {
	
		// TODO(eteran): can we use GCC's __builtin_uadd_overflow here and
		//               take advantage of hardware overflow detection? 
	
		const uint16_t temp16 = A + data + (P & C_MASK);
		const bool overflow   = ((~(A ^ data)) & (A ^ temp16) & 0x80) != 0;
		set_flag_condition<V_MASK>(overflow);
		set_flag_condition<C_MASK>(temp16 & 0x0100);
		A = temp16 & 0x00ff;
		update_nz_flags(A);
	}
};

#endif

