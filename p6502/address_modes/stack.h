
#ifndef STACK_H_
#define STACK_H_

template <class Op>
class stack {
public:
	// dispatch to the appropriate version of the address mode	
	static void execute() {
		execute(typename Op::memory_access());
	}
	
private:
	static void execute(const operation_stack_read &) {
	
		switch(cycle_) {
		case 1:
			// read next instruction byte (and throw it away)
			read_handler(PC.raw);
			break;
		case 2:
			// increment S
			++S;
			break;
		case 3:
			LAST_CYCLE;
			// pull register from stack
			Op::execute(read_handler(S + STACK_ADDRESS));
			OPCODE_COMPLETE;
		default:
			abort();
		}	
	}
	
	static void execute(const operation_stack_write &) {
	
		switch(cycle_) {
		case 1:
			// read next instruction byte (and throw it away)
			read_handler(PC.raw);
			break;
		case 2:
			LAST_CYCLE;
			// push register on stack, decrement S
			write_handler(S-- + STACK_ADDRESS, Op::execute());
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
