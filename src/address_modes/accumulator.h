
#ifndef ACCUMULATOR_H_
#define ACCUMULATOR_H_

template <class Op>
class accumulator {
public:
	// dispatch to the appropriate version of the address mode	
	static void execute() {
		execute(typename Op::memory_access());
	}
	
private:
	static void execute(const operation_modify &) {
		switch(cycle_) {
		case 1:
			LAST_CYCLE;
			// read next instruction byte (and throw it away)
			read_byte(PC.raw);
			Op::execute(A);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};


#endif
