
#ifndef IMPLIED_H_
#define IMPLIED_H_

class implied {
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Op op) {
		execute(op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Op op, const operation_none &) {
		switch(cycle_) {
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
	void execute(Op op, const operation_read &) {
		execute(op, operation_none());
	}
};

#endif
