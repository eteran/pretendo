
#ifndef INDEXED_INDIRECT_H_
#define INDEXED_INDIRECT_H_

class indexed_indirect {
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
			// fetch pointer address, increment PC
			effective_address16_.raw = PC.raw++;
			break;
		case 2:
			// read from the address, add X to it
			data8_ = read_byte(effective_address16_.raw) + X;
			break;
		case 3:
			// fetch effective address low
			effective_address16_.lo = read_byte_zp(data8_);
			break;
		case 4:
			// fetch effective address high
			effective_address16_.hi = read_byte_zp(data8_ + 1);
			break;
		case 5:
			LAST_CYCLE;
			// read from effective address
			op(read_byte(effective_address16_.raw));
			OPCODE_COMPLETE;
		default:
			abort();
			
		}
	}
	
	template <class Op>
	void execute(Op op, const operation_modify &) {

		switch(cycle_) {
		case 1:
			// fetch pointer address, increment PC
			effective_address16_.raw = PC.raw++;
			break;
		case 2:
			// read from the address, add X to it
			data8_ = read_byte(effective_address16_.raw) + X;
			break;
		case 3:
			// fetch effective address low
			effective_address16_.lo = read_byte_zp(data8_);
			break;
		case 4:
			// fetch effective address high
			effective_address16_.hi = read_byte_zp(data8_ + 1);
			break;
		case 5:
			// read from effective address
			data8_ = read_byte(effective_address16_.raw);
			break;
		case 6:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(effective_address16_.raw, data8_);
			op(data8_);
			break;
		case 7:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte(effective_address16_.raw, data8_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
		
	template <class Op>
	void execute(Op op, const operation_write &) {

		switch(cycle_) {
		case 1:
			// fetch pointer address, increment PC
			effective_address16_.raw = PC.raw++;
			break;
		case 2:
			// read from the address, add X to it
			data8_ = read_byte(effective_address16_.raw) + X;
			break;
		case 3:
			// fetch effective address low
			effective_address16_.lo = read_byte_zp(data8_);
			break;
		case 4:
			// fetch effective address high
			effective_address16_.hi = read_byte_zp(data8_ + 1);
			break;
		case 5:
			LAST_CYCLE;
			// write to effective address
			{
				const uint16_t address = effective_address16_.raw;
				const uint8_t  value   = op(address);
				write_byte(address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
