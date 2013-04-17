#ifndef NMI_20140417_H_
#define NMI_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_nmi
// Desc: Non-Maskable Interrupt
//------------------------------------------------------------------------------
struct opcode_nmi {

	void operator()(int cycle) {

		switch(cycle) {
		case 1:
			// read next instruction byte (and throw it away),
			// increment PC
			read_byte(PC);
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
			write_byte(S-- + STACK_ADDRESS, P);
			set_flag<I_MASK>();
			break;
		case 5:
			// fetch PCL
			set_pc_lo(read_byte(NMI_VECTOR_ADDRESS + 0));
			break;
		case 6:
			LAST_CYCLE;
			// fetch PCH
			set_pc_hi(read_byte(NMI_VECTOR_ADDRESS + 1));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
