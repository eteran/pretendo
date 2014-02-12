
#ifndef ACCUMULATOR_H_
#define ACCUMULATOR_H_

class accumulator {
public:
	accumulator() {
	}
	
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Context &ctx, Op op) {
		execute(ctx, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Context &ctx, Op op, const operation_modify &) {
		switch(ctx.cycle) {
		case 1:
			LAST_CYCLE;
			// read next instruction byte (and throw it away)
			read_byte(ctx, ctx.PC);
			op(ctx, ctx.A);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

private:
};


#endif
