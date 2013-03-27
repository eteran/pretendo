
#ifndef ABSOLUTE_X_H_
#define ABSOLUTE_X_H_

struct absolute_x {

	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(int cycle, Op op) {
		execute(cycle, op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(int cycle, Op op, const operation_read &) {

		switch(cycle) {
		case 1:
			// fetch low byte of address, increment PC
			effective_address_lo = read_byte(PC++);
			STAGE_END;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment PC
			effective_address_ = (read_byte(PC++) << 8);
			effective_address_lo += X;
			STAGE_END;
		case 3:

			// read from effective address,
			// fix the high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(effective_address_);

			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
				STAGE_END;
			} else {
				op(data_);
				OPCODE_COMPLETE;
			}
		case 4:
			LAST_CYCLE;
			// re-read from effective address
			op(read_byte(effective_address_));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
	template <class Op>
	void execute(int cycle, Op op, const operation_modify &) {
	
		switch(cycle) {
		case 1:
			// fetch low byte of address, increment PC
			effective_address_lo = read_byte(PC++);
			STAGE_END;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment PC
			effective_address_ = (read_byte(PC++) << 8);
			effective_address_lo += X;
			STAGE_END;
		case 3:
			// read from effective address,
			// fix the high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(effective_address_);

			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
			}
			STAGE_END;
		case 4:
			// re-read from effective address
			data_ = read_byte(effective_address_);
			STAGE_END;
		case 5:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(effective_address_, data_);
			op(data_);
			STAGE_END;
		case 6:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte(effective_address_, data_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
	template <class Op>
	void execute(int cycle, Op op, const operation_write &) {

		switch(cycle) {
		case 1:
			// fetch low byte of address, increment PC
			effective_address_lo = read_byte(PC++);
			STAGE_END;
		case 2:
			// fetch high byte of address,
			// add index register to low address byte,
			// increment PC
			effective_address_ = (read_byte(PC++) << 8);
			effective_address_lo += X;
			STAGE_END;
		case 3:
			// read from effective address,
			// fix the high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(effective_address_);

			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
			}
			STAGE_END;
		case 4:
			LAST_CYCLE;
			// write to effective address
			{
				uint16_t address = effective_address_;
				uint8_t  value   = op(address);
				write_byte(address, value);
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
