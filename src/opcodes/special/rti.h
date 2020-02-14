
#ifndef RTI_20140417_H_
#define RTI_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_rti
// Desc: Return from Interrupt
//------------------------------------------------------------------------------
class opcode_rti {
public:
	static void execute() {
		switch (cycle_) {
		case 1:
			// read next instruction byte (and throw it away)
			read_byte(PC.raw);
			break;
		case 2:
			// increment S
			++S;
			break;
		case 3:
			// pull P from stack, increment S
			P = (read_byte(S++ + StackAddress) & ~B_MASK) | R_MASK;
			break;
		case 4:
			// pull PCL from stack, increment S
			PC.lo = read_byte(S++ + StackAddress);
			break;
		case 5:
			LAST_CYCLE;
			// pull PCH from stack
			PC.hi = read_byte(S + StackAddress);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
