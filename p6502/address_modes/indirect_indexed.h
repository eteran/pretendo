
#ifndef INDIRECT_INDEXED_H_
#define INDIRECT_INDEXED_H_

struct indirect_indexed {

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
			// fetch pointer address, increment PC
			data_ = read_byte(PC++);
			break;
		case 2:
			// fetch effective address low
			effective_address_lo = read_byte_zp(data_ + 0);
			break;
		case 3:
			// fetch effective address high,
			// add Y to low byte of effective address
			effective_address_ = (read_byte_zp(data_ + 1) << 8);
			effective_address_lo += Y;
			break;
		case 4:

			// read from effective address,
			// fix high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(effective_address_);
			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
				break;
			} else {
				LAST_CYCLE;
				op(data_);
				OPCODE_COMPLETE;
			}
		case 5:
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
			// fetch pointer address, increment PC
			data_ = read_byte(PC++);
			break;
		case 2:
			// fetch effective address low
			effective_address_lo = read_byte_zp(data_ + 0);
			break;
		case 3:
			// fetch effective address high,
			// add Y to low byte of effective address
			effective_address_ = (read_byte_zp(data_ + 1) << 8);
			effective_address_lo += Y;
			break;
		case 4:

			// read from effective address,
			// fix high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(effective_address_);
			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
			}
			break;
		case 5:
			// read from effective address
			data_ = read_byte(effective_address_);
			break;
		case 6:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(effective_address_, data_);
			op(data_);
			break;
		case 7:
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
			// fetch pointer address, increment PC
			data_ = read_byte(PC++);
			break;
		case 2:
			// fetch effective address low
			effective_address_lo = read_byte_zp(data_ + 0);
			break;
		case 3:
			// fetch effective address high,
			// add Y to low byte of effective address
			effective_address_ = (read_byte_zp(data_ + 1) << 8);
			effective_address_lo += Y;
			break;
		case 4:

			// read from effective address,
			// fix high byte of effective address
			effective_address_ |= (effective_address_lo & 0xff);
			data_ = read_byte(effective_address_);
			if(effective_address_lo > 0xff) {
				effective_address_ += 0x100;
			}
			break;
		case 5:
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
