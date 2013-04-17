#ifndef RTI_20140417_H_
#define RTI_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_rti
// Desc: Return from Interrupt
//------------------------------------------------------------------------------
struct opcode_rti {

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
			// pull P from stack, increment S
			P = (read_byte(S++ + STACK_ADDRESS) & ~B_MASK) | R_MASK;
			break;
		case 4:
			// pull PCL from stack, increment S
			set_pc_lo(read_byte(S++ + STACK_ADDRESS));
			break;
		case 5:
			LAST_CYCLE;
			// pull PCH from stack
			set_pc_hi(read_byte(S + STACK_ADDRESS));
			OPCODE_COMPLETE;
		default:
			abort();

		}
	}
	
private:
};

#endif

