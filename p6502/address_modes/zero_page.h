
#ifndef ZERO_PAGE_H_
#define ZERO_PAGE_H_

class zero_page {
public:
	zero_page() {
	}
	
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Context &ctx, Op op) {
		execute(ctx, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Context &ctx, Op op, const operation_read &) {

		switch(ctx.cycle) {
		case 1:
			// fetch address, increment ctx.PC
			effective_address_ = read_byte(ctx, ctx.PC++);
			break;
		case 2:
			LAST_CYCLE;
			// read from effective address
			op(ctx, read_byte_zp(ctx, effective_address_));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	template <class Op>
	void execute(Context &ctx, Op op, const operation_modify &) {

		switch(ctx.cycle) {
		case 1:
			// fetch address, increment ctx.PC
			effective_address_ = read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// read from effective address
			data_ = read_byte_zp(ctx, effective_address_);
			break;
		case 3:
			//  write the value back to effective address,
			// and do the operation on it
			write_byte_zp(ctx, effective_address_, data_);
			op(ctx, data_);
			break;
		case 4:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte_zp(ctx, effective_address_, data_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	template <class Op>
	void execute(Context &ctx, Op op, const operation_write &) {

		switch(ctx.cycle) {
		case 1:
			// fetch address, increment ctx.PC
			effective_address_ = read_byte(ctx, ctx.PC++);
			break;
		case 2:
			LAST_CYCLE;
			// write register to effective address
			{
				uint8_t address = effective_address_;
				uint8_t value   = op(ctx, address);
            	write_byte_zp(ctx, address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

private:
	uint8_t effective_address_;
	uint8_t data_;
};

#endif
