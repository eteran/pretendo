
#ifndef ACCUMULATOR_H_
#define ACCUMULATOR_H_

class accumulator {
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Op op) {
		execute(op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Op op, const operation_modify &) {
		switch(cycle_) {
		case 1:
			LAST_CYCLE;
			// read next instruction byte (and throw it away)
			read_byte(PC);
			op(A);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};


#endif
