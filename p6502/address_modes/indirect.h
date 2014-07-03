
#ifndef INDIRECT_H_
#define INDIRECT_H_

class indirect {
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Op op) {
		execute(op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Op op, const operation_jump &) {
		switch(cycle_) {
		case 1:
			// fetch pointer address low, increment PC
			effective_address_lo_ = read_byte(PC.raw++);
			break;
		case 2:
			// fetch pointer address high, increment PC
			effective_address_hi_ =  read_byte(PC.raw++);
			break;
		case 3:
			// fetch low address byte, increment PC
			effective_address16_.lo = read_byte((effective_address_hi_ << 8) | effective_address_lo_);
			break;
		case 4:
			LAST_CYCLE;
			// fetch PCH, copy latch to PCL
			++effective_address_lo_;
			effective_address16_.hi = read_byte((effective_address_hi_ << 8) | effective_address_lo_);
			// read from effective address
			op(effective_address16_.raw);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
private:
	// effective address now contains the address to read from..
	uint8_t  effective_address_lo_;
	uint8_t  effective_address_hi_;
};

#endif
