
#ifndef ABSOLUTE_Y_H_
#define ABSOLUTE_Y_H_

class absolute_y {
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Op op) {
		execute(op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Op op, const operation_read &) {

		switch(cycle_) {
		case 1:
			// fetch low byte of address, increment PC
			data16_ = read_byte(PC++);
			break;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment PC
			effective_address16_ = (read_byte(PC++) << 8);
			data16_ += Y;
			break;
		case 3:

			// read from effective address,
			// fix the high byte of effective address
			effective_address16_ |= (data16_ & 0xff);
			data8_ = read_byte(effective_address16_);

			if(data16_ > 0xff) {
				effective_address16_ += 0x100;
				break;
			} else {
				LAST_CYCLE;
				op(data8_);
				OPCODE_COMPLETE;
			}
		case 4:
			LAST_CYCLE;
			// re-read from effective address
			op(read_byte(effective_address16_));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
	template <class Op>
	void execute(Op op, const operation_modify &) {

		switch(cycle_) {
		case 1:
			// fetch low byte of address, increment PC
			data16_ = read_byte(PC++);
			break;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment PC
			effective_address16_ = (read_byte(PC++) << 8);
			data16_ += Y;
			break;
		case 3:
			// read from effective address,
			// fix the high byte of effective address
			effective_address16_ |= (data16_ & 0xff);
			data8_ = read_byte(effective_address16_);

			if(data16_ > 0xff) {
				effective_address16_ += 0x100;
			}
			break;
		case 4:
			// re-read from effective address
			data8_ = read_byte(effective_address16_);
			break;
		case 5:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(effective_address16_, data8_);
			op(data8_);
			break;
		case 6:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte(effective_address16_, data8_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	template <class Op>
	void execute(Op op, const operation_write &) {

		switch(cycle_) {
		case 1:
			// fetch low byte of address, increment PC
			data16_ = read_byte(PC++);
			break;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment PC
			effective_address16_ = (read_byte(PC++) << 8);
			data16_ += Y;
			break;
		case 3:
			// read from effective address,
			// fix the high byte of effective address
			effective_address16_ |= (data16_ & 0xff);
			data8_ = read_byte(effective_address16_);

			if(data16_ > 0xff) {
				effective_address16_ += 0x100;
			}
			break;
		case 4:
			LAST_CYCLE;
			// write to effective address
			{
				uint16_t address = effective_address16_;
				uint8_t  value   = op(address);
				write_byte(address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
