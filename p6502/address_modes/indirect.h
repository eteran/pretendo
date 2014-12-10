
#ifndef INDIRECT_H_
#define INDIRECT_H_

template <class Op>
class indirect {
public:
	// dispatch to the appropriate version of the address mode	
	static void execute() {
		execute(typename Op::memory_access());
	}
	
private:
	static void execute(const operation_jump &) {
		switch(cycle_) {
		case 1:
			// fetch pointer address low, increment PC
			data16_.lo = read_byte(PC.raw++);
			break;
		case 2:
			// fetch pointer address high, increment PC
			data16_.hi =  read_byte(PC.raw++);
			break;
		case 3:
			// fetch low address byte, increment PC
			effective_address16_.lo = read_byte((data16_.hi << 8) | data16_.lo);
			break;
		case 4:
			LAST_CYCLE;
			// fetch PCH, copy latch to PCL
			++data16_.lo;
			effective_address16_.hi = read_byte((data16_.hi << 8) | data16_.lo);
			// read from effective address
			Op::execute(effective_address16_.raw);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
