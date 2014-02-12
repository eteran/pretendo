
#ifndef INDEXED_INDIRECT_H_
#define INDEXED_INDIRECT_H_

class indexed_indirect {
public:
	indexed_indirect() {
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
			// fetch pointer address, increment ctx.PC
			effective_address_ = ctx.PC++;
			break;
		case 2:
			// read from the address, add ctx.X to it
			data_ = read_byte(ctx, effective_address_) + ctx.X;
			break;
		case 3:
			// fetch effective address low
			effective_address_ = read_byte_zp(ctx, data_);
			break;
		case 4:
			// fetch effective address high
			effective_address_ |= (read_byte_zp(ctx, data_ + 1) << 8);
			break;
		case 5:
			LAST_CYCLE;
			// read from effective address
			op(ctx, read_byte(ctx, effective_address_));
			OPCODE_COMPLETE;
		default:
			abort();
			
		}
	}
	
	template <class Op>
	void execute(Context &ctx, Op op, const operation_modify &) {

		switch(ctx.cycle) {
		case 1:
			// fetch pointer address, increment ctx.PC
			effective_address_ = ctx.PC++;
			break;
		case 2:
			// read from the address, add ctx.X to it
			data_ = read_byte(ctx, effective_address_) + ctx.X;
			break;
		case 3:
			// fetch effective address low
			effective_address_ = read_byte_zp(ctx, data_);
			break;
		case 4:
			// fetch effective address high
			effective_address_ |= (read_byte_zp(ctx, data_ + 1) << 8);
			break;
		case 5:
			// read from effective address
			data_ = read_byte(ctx, effective_address_);
			break;
		case 6:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(ctx, effective_address_, data_);
			op(ctx, data_);
			break;
		case 7:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte(ctx, effective_address_, data_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
		
	template <class Op>
	void execute(Context &ctx, Op op, const operation_write &) {

		switch(ctx.cycle) {
		case 1:
			// fetch pointer address, increment ctx.PC
			effective_address_ = ctx.PC++;
			break;
		case 2:
			// read from the address, add ctx.X to it
			data_ = read_byte(ctx, effective_address_) + ctx.X;
			break;
		case 3:
			// fetch effective address low
			effective_address_ = read_byte_zp(ctx, data_);
			break;
		case 4:
			// fetch effective address high
			effective_address_ |= (read_byte_zp(ctx, data_ + 1) << 8);
			break;
		case 5:
			LAST_CYCLE;
			// write to effective address
			{
				uint16_t address = effective_address_;
				uint8_t  value   = op(ctx, address);
				write_byte(ctx, address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

private:
	uint16_t effective_address_;
	uint8_t  data_;
};

#endif
