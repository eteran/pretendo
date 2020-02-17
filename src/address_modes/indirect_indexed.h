
#ifndef INDIRECT_INDEXED_H_
#define INDIRECT_INDEXED_H_

template <class Op>
class indirect_indexed {
public:
	// dispatch to the appropriate version of the address mode
	static void execute() {
		execute(typename Op::memory_access());
	}

private:
	static void execute(const operation_read &) {

		switch (cycle_) {
		case 1:
			// fetch pointer address, increment PC
			data8_ = read_byte(PC.raw++);
			break;
		case 2:
			// fetch effective address low
			data16_.raw = read_byte_zp(data8_ + 0);
			break;
		case 3:
			// fetch effective address high,
			// add Y to low byte of effective address
			effective_address_.hi = read_byte_zp(data8_ + 1);
			data16_.raw += Y;
			break;
		case 4:

			// read from effective address,
			// fix high byte of effective address
			effective_address_.lo = data16_.lo;
			data8_                = read_byte(effective_address_.raw);
			if (data16_.raw > 0xff) {
				++effective_address_.hi;
				break;
			} else {
				LAST_CYCLE;
				Op::execute(data8_);
				OPCODE_COMPLETE;
			}
		case 5:
			LAST_CYCLE;
			// read from effective address
			Op::execute(read_byte(effective_address_.raw));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	static void execute(const operation_modify &) {

		switch (cycle_) {
		case 1:
			// fetch pointer address, increment PC
			data8_ = read_byte(PC.raw++);
			break;
		case 2:
			// fetch effective address low
			data16_.raw = read_byte_zp(data8_ + 0);
			break;
		case 3:
			// fetch effective address high,
			// add Y to low byte of effective address
			effective_address_.hi = read_byte_zp(data8_ + 1);
			data16_.raw += Y;
			break;
		case 4:

			// read from effective address,
			// fix high byte of effective address
			effective_address_.lo = data16_.lo;
			data8_                = read_byte(effective_address_.raw);
			if (data16_.raw > 0xff) {
				++effective_address_.hi;
			}
			break;
		case 5:
			// read from effective address
			data8_ = read_byte(effective_address_.raw);
			break;
		case 6:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(effective_address_.raw, data8_);
			Op::execute(data8_);
			break;
		case 7:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte(effective_address_.raw, data8_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}

	static void execute(const operation_write &) {

		switch (cycle_) {
		case 1:
			// fetch pointer address, increment PC
			data8_ = read_byte(PC.raw++);
			break;
		case 2:
			// fetch effective address low
			data16_.raw = read_byte_zp(data8_ + 0);
			break;
		case 3:
			// fetch effective address high,
			// add Y to low byte of effective address
			effective_address_.hi = read_byte_zp(data8_ + 1);
			data16_.raw += Y;
			break;
		case 4:

			// read from effective address,
			// fix high byte of effective address
			effective_address_.lo = data16_.lo;
			data8_                = read_byte(effective_address_.raw);
			if (data16_.raw > 0xff) {
				++effective_address_.hi;
			}
			break;
		case 5:
			LAST_CYCLE;
			// write to effective address
			{
				uint16_t address = effective_address_.raw;
				uint8_t value    = Op::execute(address);
				write_byte(address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
