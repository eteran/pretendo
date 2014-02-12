
#ifndef ABSOLUTE_X_H_
#define ABSOLUTE_X_H_

class absolute_x {
public:
	absolute_x() {
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
			// fetch low byte of address, increment ctx.PC
			effective_address_lo = read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment ctx.PC
			effective_address_ = (read_byte(ctx, ctx.PC++) << 8);
			effective_address_lo += ctx.X;
			break;
		case 3:

			// read from effective address,
			// fix the high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(ctx, effective_address_);

			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
				break;
			} else {
				op(ctx, data_);
				OPCODE_COMPLETE;
			}
		case 4:
			LAST_CYCLE;
			// re-read from effective address
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
			// fetch low byte of address, increment ctx.PC
			effective_address_lo = read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment ctx.PC
			effective_address_ = (read_byte(ctx, ctx.PC++) << 8);
			effective_address_lo += ctx.X;
			break;
		case 3:
			// read from effective address,
			// fix the high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(ctx, effective_address_);

			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
			}
			break;
		case 4:
			// re-read from effective address
			data_ = read_byte(ctx, effective_address_);
			break;
		case 5:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(ctx, effective_address_, data_);
			op(ctx, data_);
			break;
		case 6:
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
			// fetch low byte of address, increment ctx.PC
			effective_address_lo = read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment ctx.PC
			effective_address_ = (read_byte(ctx, ctx.PC++) << 8);
			effective_address_lo += ctx.X;
			break;
		case 3:
			// read from effective address,
			// fix the high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(ctx, effective_address_);

			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
			}
			break;
		case 4:
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
	uint16_t effective_address_lo;
	uint8_t  data_;
};

#endif
