
#ifndef IMMEDIATE_H_
#define IMMEDIATE_H_

class immediate {
public:
	immediate() {
	}
	
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Context &ctx, Op op) {
		execute(ctx, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Context &ctx, Op op, const operation_read &) {
		switch(ctx.cycle) {
		case 1:
			LAST_CYCLE;
			// fetch value, increment ctx.PC
			op(ctx, read_byte(ctx, ctx.PC++));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
private:
};

#endif
