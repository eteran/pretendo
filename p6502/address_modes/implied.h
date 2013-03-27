
#ifndef IMPLIED_H_
#define IMPLIED_H_

struct implied {

	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(int cycle, Op op) {
		execute(cycle, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(int cycle, Op op, const operation_none &) {
		switch(cycle) {
		case 1:
			LAST_CYCLE;
			// read next instruction byte (and throw it away)
			read_byte(PC);
			op();
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
	// really this is only for NOP support...
	// synonym for other version
	template <class Op>
	void execute(int cycle, Op op, const operation_read &) {
		execute(cycle, op, operation_none());
	}
};

#endif
