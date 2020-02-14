
#ifndef ZERO_PAGE_H_
#define ZERO_PAGE_H_

template <class Op>
class zero_page {
public:
	// dispatch to the appropriate version of the address mode
	static void execute() {
		execute(typename Op::memory_access());
	}

private:
	static void execute(const operation_read &) {

		switch (cycle_) {
		case 1:
			// fetch address, increment PC
			effective_address16_.raw = read_byte(PC.raw++);
			break;
		case 2:
			LAST_CYCLE;
			// read from effective address
			Op::execute(read_byte_zp(effective_address16_.lo));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	static void execute(const operation_modify &) {

		switch (cycle_) {
		case 1:
			// fetch address, increment PC
			effective_address16_.raw = read_byte(PC.raw++);
			break;
		case 2:
			// read from effective address
			data8_ = read_byte_zp(effective_address16_.lo);
			break;
		case 3:
			//  write the value back to effective address,
			// and do the operation on it
			write_byte_zp(effective_address16_.lo, data8_);
			Op::execute(data8_);
			break;
		case 4:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte_zp(effective_address16_.lo, data8_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	static void execute(const operation_write &) {

		switch (cycle_) {
		case 1:
			// fetch address, increment PC
			effective_address16_.raw = read_byte(PC.raw++);
			break;
		case 2:
			LAST_CYCLE;
			// write register to effective address
			{
				const uint8_t address = effective_address16_.lo;
				const uint8_t value   = Op::execute(address);
				write_byte_zp(address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
