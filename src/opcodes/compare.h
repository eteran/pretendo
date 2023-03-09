#ifndef COMPARE_20121206_H_
#define COMPARE_20121206_H_

struct compare {
	using memory_access = operation_read;

protected:
	static void do_compare(uint8_t reg, uint8_t data) {
		// compares a register to internal 8-bit register
		// and sets flags
		const uint_least16_t data16 = reg - data;
		set_flag_condition<C_MASK>((data16 & 0x8000) == 0);
		data = static_cast<uint8_t>(data16);
		update_nz_flags(data);
	}
};

#endif
