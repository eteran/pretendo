
#ifndef ABSOLUTE_H_
#define ABSOLUTE_H_

class absolute {
public:
	absolute() {
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
			// fetch low address byte, increment ctx.PC
			effective_address_ = (effective_address_ & 0xff00) | read_byte(ctx, ctx.PC++);
			break;
		case 2:
			LAST_CYCLE;
			// copy low address byte to PCL, fetch high address
			// byte to PCH
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(ctx, ctx.PC++) << 8);
			op(ctx, effective_address_);
			OPCODE_COMPLETE;
		default:
			abort();	
		}
	}
                      
	
	template <class Op>
	void execute(Context &ctx, Op op, const operation_read &) {

		switch(ctx.cycle) {
		case 1:
			// fetch low byte of address, increment ctx.PC
			effective_address_ = (effective_address_ & 0xff00) | read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// fetch high byte of address, increment ctx.PC
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(ctx, ctx.PC++) << 8);
			break;
		case 3:
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
			// fetch low byte of address, increment ctx.PC
			effective_address_ = (effective_address_ & 0xff00) | read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// fetch high byte of address, increment ctx.PC
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(ctx, ctx.PC++) << 8);
			break;
		case 3:
			// read from effective address
			data_ = read_byte(ctx, effective_address_);
			break;
		case 4:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(ctx, effective_address_, data_);
			op(ctx, data_);
			break;
		case 5:
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
			effective_address_ = (effective_address_ & 0xff00) | read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// fetch high byte of address, increment ctx.PC
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(ctx, ctx.PC++) << 8);
			break;
		case 3:
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
