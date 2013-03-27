
#ifndef INDIRECT_H_
#define INDIRECT_H_

struct indirect {

	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(int cycle, Op op) {
		execute(cycle, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(int cycle, Op op, const operation_jump &) {
		switch(cycle) {
		case 1:
			// fetch pointer address low, increment PC
			effective_address_lo_ = read_byte(PC++);
			STAGE_END;
		case 2:
			// fetch pointer address high, increment PC
			effective_address_hi_ =  read_byte(PC++);
			STAGE_END;
		case 3:
			// fetch low address byte, increment PC
			effective_address2_ = (effective_address2_ & 0xff00) | read_byte((effective_address_hi_ << 8) | effective_address_lo_);
			STAGE_END;
		case 4:
			LAST_CYCLE;
			// fetch PCH, copy latch to PCL
			++effective_address_lo_;
			effective_address2_ = (effective_address2_ & 0x00ff) | (read_byte((effective_address_hi_ << 8) | effective_address_lo_) << 8);
			// read from effective address
			op(effective_address2_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
private:
	// effective address now contains the address to read from..
	uint16_t effective_address2_;
	uint8_t  effective_address_lo_;
	uint8_t  effective_address_hi_;
};

#endif
