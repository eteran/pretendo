
#ifndef ZERO_PAGE_H_
#define ZERO_PAGE_H_

class zero_page {
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
			effective_address16_ = read_byte(PC++);
			break;
		case 2:
			LAST_CYCLE;
			// read from effective address
			op(read_byte_zp(effective_address16_));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	template <class Op>
	void execute(Op op, const operation_modify &) {

		switch(cycle_) {
		case 1:
			// fetch address, increment PC
			effective_address16_ = read_byte(PC++);
			break;
		case 2:
			// read from effective address
			data8_ = read_byte_zp(effective_address16_);
			break;
		case 3:
			//  write the value back to effective address,
			// and do the operation on it
			write_byte_zp(effective_address16_, data8_);
			op(data8_);
			break;
		case 4:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte_zp(effective_address16_, data8_);
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
			effective_address16_ = read_byte(PC++);
			break;
		case 2:
			LAST_CYCLE;
			// write register to effective address
			{
				uint8_t address = effective_address16_;
				uint8_t value   = op(address);
            	write_byte_zp(address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
