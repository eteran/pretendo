
#ifndef STACK_H_
#define STACK_H_

class stack {
public:
	stack() {
	}
	
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Context &ctx, Op op) {
		execute(ctx, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Context &ctx, Op op, const operation_stack_read &) {
	
		switch(ctx.cycle) {
		case 1:
			// read next instruction byte (and throw it away)
			read_byte(ctx, ctx.PC);
			break;
		case 2:
			// increment ctx.S
			++ctx.S;
			break;
		case 3:
			LAST_CYCLE;
			// pull register from stack
			op(ctx, read_byte(ctx, ctx.S + STACK_ADDRESS));
			OPCODE_COMPLETE;
		default:
			abort();
		}	
	}
	
	template <class Op>
	void execute(Context &ctx, Op op, const operation_stack_write &) {
	
		switch(ctx.cycle) {
		case 1:
			// read next instruction byte (and throw it away)
			read_byte(ctx, ctx.PC);
			break;
		case 2:
			LAST_CYCLE;
			// push register on stack, decrement ctx.S
			write_byte(ctx, ctx.S-- + STACK_ADDRESS, op(ctx));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

private:
};

#endif
