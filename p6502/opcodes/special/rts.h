
#ifndef RTS_20140417_H_
#define RTS_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_rts
// Desc: Return from Subroutine
//------------------------------------------------------------------------------
class opcode_rts {
public:
	opcode_rts() {
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
			// pull PCL from stack, increment S
			set_pc_lo(ctx, read_byte(ctx, ctx.S++ + STACK_ADDRESS));
			break;
		case 4:
			// pull PCH from stack
			set_pc_hi(ctx, read_byte(ctx, ctx.S + STACK_ADDRESS));
			break;
		case 5:
			LAST_CYCLE;
			 // increment PC
			++ctx.PC;
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif

