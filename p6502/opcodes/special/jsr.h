
#ifndef JSR_20140417_H_
#define JSR_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_jsr
// Desc: Jump to Subroutine
//------------------------------------------------------------------------------
class opcode_jsr {
public:
	opcode_jsr() {
	}
	
public:
	void operator()(Context &ctx) {
		execute(ctx);
	}

private:
	void execute(Context &ctx) {
		switch(ctx.cycle) {
		case 1:
			// fetch low address byte, increment PC
			effective_address_ = (effective_address_ & 0xff00) | read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// internal operation (predecrement S?)
			break;
		case 3:
			// push PCH on stack, decrement S
			write_byte(ctx, ctx.S-- + STACK_ADDRESS, pc_hi(ctx));
			break;
		case 4:
			// push PCL on stack, decrement S
			write_byte(ctx, ctx.S-- + STACK_ADDRESS, pc_lo(ctx));
			break;
		case 5:
			LAST_CYCLE;
			// fetch high address byte to PCH
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(ctx, ctx.PC) << 8);
			ctx.PC = effective_address_;
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
private:
	uint16_t effective_address_;
};

#endif

