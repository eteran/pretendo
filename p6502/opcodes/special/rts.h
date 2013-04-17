#ifndef RTS_20140417_H_
#define RTS_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_rts
// Desc: Return from Subroutine
//------------------------------------------------------------------------------
struct opcode_rts {

	void operator()(int cycle) {
		switch(cycle) {
		case 1:
			// read next instruction byte (and throw it away)
			read_byte(PC);
			break;
		case 2:
			// increment S
			++S;
			break;
		case 3:
			// pull PCL from stack, increment S
			set_pc_lo(read_byte(S++ + STACK_ADDRESS));
			break;
		case 4:
			// pull PCH from stack
			set_pc_hi(read_byte(S + STACK_ADDRESS));
			break;
		case 5:
			LAST_CYCLE;
			 // increment PC
			++PC;
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif

