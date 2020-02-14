
#ifndef IRQ_20140417_H_
#define IRQ_20140417_H_

#define LAST_CYCLE_0                                       \
	do {                                                   \
		rst_executing_ = false;                            \
		irq_executing_ = false;                            \
                                                           \
		if (rst_asserted_) {                               \
			rst_executing_ = true;                         \
		} else if (irq_asserted_ && ((P & I_MASK) == 0)) { \
			irq_executing_ = true;                         \
		}                                                  \
		rst_asserted_ = false;                             \
	} while (0)

//------------------------------------------------------------------------------
// Name: opcode_irq
// Desc: Interrupt
//------------------------------------------------------------------------------
class opcode_irq {
public:
	static void execute() {
		switch (cycle_) {
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
			if (nmi_asserted_) {
				effective_address16_.raw = NmiVectorAddress;
				nmi_asserted_            = false;
			} else {
				effective_address16_.raw = IrqVectorAddress;
			}
			break;
		case 5:
			set_flag<I_MASK>();
			// fetch PCL
			PC.lo = read_byte(effective_address16_.raw + 0);
			break;
		case 6:
			LAST_CYCLE_0;
			// fetch PCH
			PC.hi = read_byte(effective_address16_.raw + 1);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#undef LAST_CYCLE_0

#endif
