
#ifndef NMI_20140417_H_
#define NMI_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_nmi
// Desc: Non-Maskable Interrupt
//------------------------------------------------------------------------------
class opcode_nmi {
public:
	opcode_nmi() {
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
			break;
		case 5:
			set_flag<I_MASK>(ctx);
			// fetch PCL
			set_pc_lo(ctx, read_byte(ctx, NMI_VECTOR_ADDRESS + 0));
			break;
		case 6:
			LAST_CYCLE;
			// fetch PCH
			set_pc_hi(ctx, read_byte(ctx, NMI_VECTOR_ADDRESS + 1));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
