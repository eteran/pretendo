#ifndef ASR_20121206_H_
#define ASR_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_asr
// Desc: Logical AND then LSR A
//------------------------------------------------------------------------------
struct opcode_asr {

	typedef operation_read memory_access;

	static void execute(uint8_t data) {
		A &= data;
		set_flag_condition<C_MASK>(A & 0x01);
		A >>= 1;
		update_nz_flags(A);
	}
};

#endif

