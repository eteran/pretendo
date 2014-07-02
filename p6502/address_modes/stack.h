
#ifndef STACK_H_
#define STACK_H_

class stack {
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Op op) {
		execute(op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Op op, const operation_stack_read &) {
	
		switch(cycle_) {
		case 1:
			// read next instruction byte (and throw it away)
			read_byte(PC);
			break;
		case 2:
			// increment S
			++S;
			break;
		case 3:
			LAST_CYCLE;
			// pull register from stack
			op(read_byte(S + STACK_ADDRESS));
			OPCODE_COMPLETE;
		default:
			abort();
		}	
	}
	
	template <class Op>
	void execute(Op op, const operation_stack_write &) {
	
		switch(cycle_) {
		case 1:
			// read next instruction byte (and throw it away)
			read_byte(PC);
			break;
		case 2:
			LAST_CYCLE;
			// push register on stack, decrement S
			write_byte(S-- + STACK_ADDRESS, op());
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
