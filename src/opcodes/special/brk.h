
#ifndef BRK_20140417_H_
#define BRK_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_brk
// Desc: Software Interrupt
//------------------------------------------------------------------------------
class opcode_brk {
public:
	static void execute() {
		switch (cycle_) {
		case 1:
			// read next instruction byte (and throw it away),
			// increment PC
			read_byte(PC.raw++);
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
			write_byte(S-- + StackAddress, P | B_MASK);
			if (nmi_asserted_) {
				effective_address_.raw = NmiVectorAddress;
				nmi_asserted_          = false;
			} else {
				effective_address_.raw = IrqVectorAddress;
			}
			break;
		case 5:
			set_flag<I_MASK>();
			// fetch PCL
			PC.lo = read_byte(effective_address_.raw + 0);
			break;
		case 6:
			// NOTE: are we supposed to check for interrupts here?
			//       I only pass the tests if I don't.
			//LAST_CYCLE;

			// fetch PCH
			PC.hi = read_byte(effective_address_.raw + 1);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
