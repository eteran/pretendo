#ifndef JSR_20140417_H_
#define JSR_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_jsr
// Desc: Jump to Subroutine
//------------------------------------------------------------------------------
struct opcode_jsr {

	void operator()(int cycle) {
		switch(cycle) {
		case 1:
			// fetch low address byte, increment PC
			effective_address_ = (effective_address_ & 0xff00) | read_byte(PC++);
			break;
		case 2:
			// internal operation (predecrement S?)
			break;
		case 3:
			// push PCH on stack, decrement S
			write_byte(S-- + STACK_ADDRESS, pc_hi());
			break;
		case 4:
			// push PCL on stack, decrement S
			write_byte(S-- + STACK_ADDRESS, pc_lo());
			break;
		case 5:
			LAST_CYCLE;
			// fetch high address byte to PCH
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(PC) << 8);
			PC = effective_address_;
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
private:
	uint16_t effective_address_;
};

#endif

