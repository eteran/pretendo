
#ifndef NMI_20140417_H_
#define NMI_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_nmi
// Desc: Non-Maskable Interrupt
//------------------------------------------------------------------------------
class opcode_nmi {
public:
	static void execute() {
		switch(cycle_) {
		case 1:
			// read next instruction byte (and throw it away),
			// increment PC
			read_byte(PC.raw);
			break;
		case 2:
			// push PCH on stack, decrement S
			write_byte(S-- + StackAddress, PC.hi);
			break;
		case 3:
			// push PCL on stack, decrement S
			write_byte(S-- + StackAddress, PC.lo);
			break;
		case 4:
			// push P on stack, decrement S
			write_byte(S-- + StackAddress, P);
			break;
		case 5:
			set_flag<I_MASK>();
			// fetch PCL
			PC.lo = read_byte(NmiVectorAddress + 0);
			break;
		case 6:
			LAST_CYCLE;
			// fetch PCH
			PC.hi = read_byte(NmiVectorAddress + 1);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
