
#ifndef ABSOLUTE_H_
#define ABSOLUTE_H_

struct absolute {

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
			// fetch low address byte, increment PC
			effective_address_ = (effective_address_ & 0xff00) | read_byte(PC++);
			STAGE_END;
		case 2:
			LAST_CYCLE;
			// copy low address byte to PCL, fetch high address
			// byte to PCH
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(PC++) << 8);
			op(effective_address_);
			OPCODE_COMPLETE;
		default:
			abort();	
		}
	}
                      
	
	template <class Op>
	void execute(int cycle, Op op, const operation_read &) {

		switch(cycle) {
		case 1:
			// fetch low byte of address, increment PC
			effective_address_ = (effective_address_ & 0xff00) | read_byte(PC++);
			STAGE_END;
		case 2:
			// fetch high byte of address, increment PC
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(PC++) << 8);
			STAGE_END;
		case 3:
			LAST_CYCLE;
			// read from effective address
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
			effective_address_ = (effective_address_ & 0xff00) | read_byte(PC++);
			STAGE_END;
		case 2:
			// fetch high byte of address, increment PC
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(PC++) << 8);
			STAGE_END;
		case 3:
			// read from effective address
			data_ = read_byte(effective_address_);
			STAGE_END;
		case 4:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(effective_address_, data_);
			op(data_);
			STAGE_END;
		case 5:
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
			effective_address_ = (effective_address_ & 0xff00) | read_byte(PC++);
			STAGE_END;
		case 2:
			// fetch high byte of address, increment PC
			effective_address_ = (effective_address_ & 0x00ff) | (read_byte(PC++) << 8);
			STAGE_END;
		case 3:
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
	uint8_t  data_;
};

#endif
