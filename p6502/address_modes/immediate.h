
#ifndef IMMEDIATE_H_
#define IMMEDIATE_H_

template <class Op>
class immediate {
public:
	// dispatch to the appropriate version of the address mode	
	static void execute() {
		execute(typename Op::memory_access());
	}
	
private:
	static void execute(const operation_read &) {
		switch(cycle_) {
		case 1:
			LAST_CYCLE;
			// fetch value, increment PC
			Op::execute(read_handler(PC.raw++));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
