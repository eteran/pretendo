
#ifndef IMPLIED_H_
#define IMPLIED_H_

class implied {
public:
	implied() {
	}
	
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Context &ctx, Op op) {
		execute(ctx, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Context &ctx, Op op, const operation_none &) {
		switch(ctx.cycle) {
		case 1:
			LAST_CYCLE;
			// read next instruction byte (and throw it away)
			read_byte(ctx, ctx.PC);
			op(ctx);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
	// really this is only for NOP support...
	// synonym for other version
	template <class Op>
	void execute(Context &ctx, Op op, const operation_read &) {
		execute(ctx, op, operation_none());
	}

private:
};

#endif
