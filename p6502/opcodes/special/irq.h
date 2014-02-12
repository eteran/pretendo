
#ifndef IRQ_20140417_H_
#define IRQ_20140417_H_

#define LAST_CYCLE_0                                         \
do {                                                         \
	ctx.rst_executing = false;                               \
	ctx.irq_executing = false;                               \
													         \
	if(ctx.rst_asserted) {                                   \
		ctx.rst_executing = true;                            \
	} else if(ctx.irq_asserted && ((ctx.P & I_MASK) == 0)) { \
		ctx.irq_executing = true;                            \
	}                                                        \
	ctx.rst_asserted = false;                                \
} while(0)

//------------------------------------------------------------------------------
// Name: opcode_irq
// Desc: Interrupt
//------------------------------------------------------------------------------
class opcode_irq {
public:
	opcode_irq() {
	}
	
public:
	void operator()(Context &ctx) {
		execute(ctx);
	}

private:
	void execute(Context &ctx) {

		switch(ctx.cycle) {
		case 1:
			// read next instruction byte (and throw it away),
			// increment PC
			read_byte(ctx, ctx.PC);
			break;
		case 2:
			// push PCH on stack, decrement S
			write_byte(ctx, ctx.S-- + STACK_ADDRESS, pc_hi(ctx));
			break;
		case 3:
			// push PCL on stack, decrement S
			write_byte(ctx, ctx.S-- + STACK_ADDRESS, pc_lo(ctx));
			break;
		case 4:
			// push P on stack, decrement S
			write_byte(ctx, ctx.S-- + STACK_ADDRESS, ctx.P);
			if(ctx.nmi_asserted) {
				vector_ = NMI_VECTOR_ADDRESS;
				ctx.nmi_asserted = false;
			} else {
				vector_ = IRQ_VECTOR_ADDRESS;
			}
			break;
		case 5:
			set_flag<I_MASK>(ctx);
			// fetch PCL
			set_pc_lo(ctx, read_byte(ctx, vector_ + 0));
			break;
		case 6:
			LAST_CYCLE_0;
			// fetch PCH
			set_pc_hi(ctx, read_byte(ctx, vector_ + 1));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

private:
	uint16_t vector_;
};

#undef LAST_CYCLE_0

#endif

