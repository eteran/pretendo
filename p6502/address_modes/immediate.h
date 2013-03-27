
#ifndef IMMEDIATE_H_
#define IMMEDIATE_H_

struct immediate {

	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(int cycle, Op op) {
		execute(cycle, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(int cycle, Op op, const operation_read &) {
		switch(cycle) {
		case 1:
			LAST_CYCLE;
			// fetch value, increment PC
			op(read_byte(PC++));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
