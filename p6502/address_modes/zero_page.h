
#ifndef ZERO_PAGE_H_
#define ZERO_PAGE_H_

struct zero_page {

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
			// fetch address, increment PC
			effective_address_ = read_byte(PC++);
			STAGE_END;
		case 2:
			LAST_CYCLE;
			// read from effective address
			op(read_byte_zp(effective_address_));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	template <class Op>
	void execute(int cycle, Op op, const operation_modify &) {

		switch(cycle) {
		case 1:
			// fetch address, increment PC
			effective_address_ = read_byte(PC++);
			STAGE_END;
		case 2:
			// read from effective address
			data_ = read_byte_zp(effective_address_);
			STAGE_END;
		case 3:
			//  write the value back to effective address,
			// and do the operation on it
			write_byte_zp(effective_address_, data_);
			op(data_);
			STAGE_END;
		case 4:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte_zp(effective_address_, data_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	template <class Op>
	void execute(int cycle, Op op, const operation_write &) {

		switch(cycle) {
		case 1:
			// fetch address, increment PC
			effective_address_ = read_byte(PC++);
			STAGE_END;
		case 2:
			LAST_CYCLE;
			// write register to effective address
			{
				uint8_t address = effective_address_;
				uint8_t value   = op(address);
            	write_byte_zp(address, value);
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
