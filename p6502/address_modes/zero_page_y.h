
#ifndef ZERO_PAGE_Y_H_
#define ZERO_PAGE_Y_H_

struct zero_page_y {

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
			effective_address_ = PC++;
			STAGE_END;
		case 2:
			// read from address, add index register to it
			effective_address_ = read_byte(effective_address_) + Y;
			STAGE_END;
		case 3:
			LAST_CYCLE;
			// read from effective address
            op(read_byte_zp(effective_address_ & 0xff));
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
			effective_address_ = PC++;
			STAGE_END;
		case 2:
			// read from address, add index register to it
			effective_address_ = read_byte(effective_address_) + Y;
			STAGE_END;
		case 3:
			LAST_CYCLE;
			// write to effective address
			{
				uint16_t address = effective_address_ & 0xff;
				uint8_t  value   = op(address);
            	write_byte_zp(address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

private:
	uint16_t effective_address_;
};

#endif
