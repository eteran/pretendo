
#ifndef BRK_20140417_H_
#define BRK_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_brk
// Desc: Software Interrupt
//------------------------------------------------------------------------------
class opcode_brk {
public:
	void operator()() {
		execute();
	}

private:
	void execute() {
		switch(cycle_) {
		case 1:
			// read next instruction byte (and throw it away),
			// increment PC
			read_byte(PC++);
			break;
		case 2:
			// push PCH on stack, decrement S
			write_byte(S-- + STACK_ADDRESS, pc_hi());
			break;
		case 3:
			// push PCL on stack, decrement S
			write_byte(S-- + STACK_ADDRESS, pc_lo());
			break;
		case 4:
			// push P on stack, decrement S
			write_byte(S-- + STACK_ADDRESS, P | B_MASK);
			if(nmi_asserted_) {
				effective_address16_ = NMI_VECTOR_ADDRESS;
				nmi_asserted_ = false;
			} else {
				effective_address16_ = IRQ_VECTOR_ADDRESS;
			}
			break;
		case 5:
			set_flag<I_MASK>();
			// fetch PCL
			set_pc_lo(read_byte(effective_address16_ + 0));
			break;
		case 6:
			// NOTE: are we supposed to check for interrupts here?
			//       I only pass the tests if I don't.
			//LAST_CYCLE;

			// fetch PCH
			set_pc_hi(read_byte(effective_address16_ + 1));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif

