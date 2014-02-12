
#ifndef RST_20140417_H_
#define RST_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_rst
// Desc: Reset
//------------------------------------------------------------------------------
class opcode_rst {
public:
	opcode_rst() {
	}
	
public:
	void operator()(Context &ctx) {
		execute(ctx);
	}

private:
	void execute(Context &ctx) {
		switch(ctx.cycle) {
		case 1:
			// read from current ctx.PC
			read_byte(ctx, ctx.PC);
			break;
		case 2:
			// push PCH on stack, decrement ctx.S (fake)
			read_byte(ctx, ctx.S-- + STACK_ADDRESS);
			break;
		case 3:
			// push PCL on stack, decrement ctx.S (fake)
			read_byte(ctx, ctx.S-- + STACK_ADDRESS);
			break;
		case 4:
			// push ctx.P on stack, decrement ctx.S (fake)
			read_byte(ctx, ctx.S-- + STACK_ADDRESS);		
			break;
		case 5:
			set_flag<I_MASK>(ctx);
			// fetch PCL
			set_pc_lo(ctx, read_byte(ctx, RST_VECTOR_ADDRESS + 0));
			break;
		case 6:
			// fetch PCH
			LAST_CYCLE;
			set_pc_hi(ctx, read_byte(ctx, RST_VECTOR_ADDRESS + 1));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
