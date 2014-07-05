
#ifndef IMMEDIATE_H_
#define IMMEDIATE_H_

class immediate {
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Op op) {
		execute(op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Op op, const operation_read &) {
		switch(cycle_) {
		case 1:
			LAST_CYCLE;
			// fetch value, increment PC
			op(read_byte(PC.raw++));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
