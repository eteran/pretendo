#ifndef AXS_20121206_H_
#define AXS_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_axs
// Desc: Transfer A to X
//------------------------------------------------------------------------------
struct opcode_axs {
	using memory_access = operation_read;

	static void execute(uint8_t data) {
		const uint_least16_t new_x = (X & A) - data;
		const bool carry           = (new_x < 0x100);
		X                          = new_x & 0xff;
		update_nz_flags(X);
		set_flag_condition<C_MASK>(carry);
	}
};

#endif
