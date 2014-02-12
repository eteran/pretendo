
#ifndef INDIRECT_H_
#define INDIRECT_H_

class indirect {
public:
	indirect() {
	}
	
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Context &ctx, Op op) {
		execute(ctx, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Context &ctx, Op op, const operation_jump &) {
		switch(ctx.cycle) {
		case 1:
			// fetch pointer address low, increment ctx.PC
			effective_address_lo_ = read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// fetch pointer address high, increment ctx.PC
			effective_address_hi_ =  read_byte(ctx, ctx.PC++);
			break;
		case 3:
			// fetch low address byte, increment ctx.PC
			effective_address2_ = (effective_address2_ & 0xff00) | read_byte(ctx, (effective_address_hi_ << 8) | effective_address_lo_);
			break;
		case 4:
			LAST_CYCLE;
			// fetch PCH, copy latch to PCL
			++effective_address_lo_;
			effective_address2_ = (effective_address2_ & 0x00ff) | (read_byte(ctx, (effective_address_hi_ << 8) | effective_address_lo_) << 8);
			// read from effective address
			op(ctx, effective_address2_);
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
