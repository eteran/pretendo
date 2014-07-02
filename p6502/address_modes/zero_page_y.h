
#ifndef ZERO_PAGE_Y_H_
#define ZERO_PAGE_Y_H_

class zero_page_y {
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
			// fetch address, increment PC
			effective_address16_ = PC++;
			break;
		case 2:
			// read from address, add index register to it
			effective_address16_ = read_byte(effective_address16_) + Y;
			break;
		case 3:
			LAST_CYCLE;
			// read from effective address
            op(read_byte_zp(effective_address16_ & 0xff));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	template <class Op>
	void execute(Op op, const operation_write &) {

		switch(cycle_) {
		case 1:
			// fetch address, increment PC
			effective_address16_ = PC++;
			break;
		case 2:
			// read from address, add index register to it
			effective_address16_ = read_byte(effective_address16_) + Y;
			break;
		case 3:
			LAST_CYCLE;
			// write to effective address
			{
				uint16_t address = effective_address16_ & 0xff;
				uint8_t  value   = op(address);
            	write_byte_zp(address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
