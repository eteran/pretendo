
#ifndef RTI_20140417_H_
#define RTI_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_rti
// Desc: Return from Interrupt
//------------------------------------------------------------------------------
class opcode_rti {
public:
	opcode_rti() {
	}
	
public:
	void operator()(Context &ctx) {
		execute(ctx);
	}

private:
	void execute(Context &ctx) {
		switch(ctx.cycle) {
		case 1:
			// read next instruction byte (and throw it away)
			read_byte(ctx, ctx.PC);
			break;
		case 2:
			// increment S
			++ctx.S;
			break;
		case 3:
			// pull P from stack, increment S
			ctx.P = (read_byte(ctx, ctx.S++ + STACK_ADDRESS) & ~B_MASK) | R_MASK;
			break;
		case 4:
			// pull PCL from stack, increment S
			set_pc_lo(ctx, read_byte(ctx, ctx.S++ + STACK_ADDRESS));
			break;
		case 5:
			LAST_CYCLE;
			// pull PCH from stack
			set_pc_hi(ctx, read_byte(ctx, ctx.S + STACK_ADDRESS));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif

