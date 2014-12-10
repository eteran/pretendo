#ifndef COMPARE_20121206_H_
#define COMPARE_20121206_H_

struct compare {
	typedef operation_read memory_access;

protected:
	static void do_compare(uint8_t reg, uint8_t data) {
		// compares a register to internal 8-bit register
		// and sets flags
		const uint16_t data16 = reg - data;
		set_flag_condition<C_MASK>((data16 & 0x8000) == 0);
		data = static_cast<uint8_t>(data16);
		update_nz_flags(data);
	}
};

#endif

