
#ifndef JSR_20140417_H_
#define JSR_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_jsr
// Desc: Jump to Subroutine
//------------------------------------------------------------------------------
class opcode_jsr {
public:
	static void execute() {
		switch(cycle_) {
		case 1:
			// fetch low address byte, increment PC
			effective_address16_.lo = read_byte(PC.raw++);
			break;
		case 2:
			// internal operation (predecrement S?)
			break;
		case 3:
			// push PCH on stack, decrement S
			write_byte(S-- + StackAddress, PC.hi);
			break;
		case 4:
			// push PCL on stack, decrement S
			write_byte(S-- + StackAddress, PC.lo);
			break;
		case 5:
			LAST_CYCLE;
			// fetch high address byte to PCH
			effective_address16_.hi = read_byte(PC.raw);
			PC.raw = effective_address16_.raw;
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif

