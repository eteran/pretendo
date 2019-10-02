
#ifndef RTS_20140417_H_
#define RTS_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_rts
// Desc: Return from Subroutine
//------------------------------------------------------------------------------
class opcode_rts {
public:
	static void execute() {
		switch(cycle_) {
		case 1:
			// read next instruction byte (and throw it away)
			read_byte(PC.raw);
			break;
		case 2:
			// increment S
			++S;
			break;
		case 3:
			// pull PCL from stack, increment S
			PC.lo = read_byte(S++ + StackAddress);
			break;
		case 4:
			// pull PCH from stack
			PC.hi = read_byte(S + StackAddress);
			break;
		case 5:
			LAST_CYCLE;
			 // increment PC
			++PC.raw;
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif

