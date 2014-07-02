
#ifndef ABSOLUTE_H_
#define ABSOLUTE_H_

class absolute {
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Op op) {
		execute(op, typename Op::memory_access());
	}
	
private:
	template <class Op>
	void execute(Op op, const operation_jump &) {
		switch(cycle_) {
		case 1:
			// fetch low address byte, increment PC
			effective_address16_ = (effective_address16_ & 0xff00) | read_byte(PC++);
			break;
		case 2:
			LAST_CYCLE;
			// copy low address byte to PCL, fetch high address
			// byte to PCH
			effective_address16_ = (effective_address16_ & 0x00ff) | (read_byte(PC++) << 8);
			op(effective_address16_);
			OPCODE_COMPLETE;
		default:
			abort();	
		}
	}
                      
	
	template <class Op>
	void execute(Op op, const operation_read &) {

		switch(cycle_) {
		case 1:
			// fetch low byte of address, increment PC
			effective_address16_ = (effective_address16_ & 0xff00) | read_byte(PC++);
			break;
		case 2:
			// fetch high byte of address, increment PC
			effective_address16_ = (effective_address16_ & 0x00ff) | (read_byte(PC++) << 8);
			break;
		case 3:
			LAST_CYCLE;
			// read from effective address
			op(read_byte(effective_address16_));
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
	template <class Op>
	void execute(Op op, const operation_modify &) {

		switch(cycle_) {
		case 1:
			// fetch low byte of address, increment PC
			effective_address16_ = (effective_address16_ & 0xff00) | read_byte(PC++);
			break;
		case 2:
			// fetch high byte of address, increment PC
			effective_address16_ = (effective_address16_ & 0x00ff) | (read_byte(PC++) << 8);
			break;
		case 3:
			// read from effective address
			data8_ = read_byte(effective_address16_);
			break;
		case 4:
			// write the value back to effective address,
			// and do the operation on it
			write_byte(effective_address16_, data8_);
			op(data8_);
			break;
		case 5:
			LAST_CYCLE;
			// write the new value to effective address
			write_byte(effective_address16_, data8_);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
	
	template <class Op>
	void execute(Op op, const operation_write &) {

		switch(cycle_) {
		case 1:
			// fetch low byte of address, increment PC
			effective_address16_ = (effective_address16_ & 0xff00) | read_byte(PC++);
			break;
		case 2:
			// fetch high byte of address, increment PC
			effective_address16_ = (effective_address16_ & 0x00ff) | (read_byte(PC++) << 8);
			break;
		case 3:
			LAST_CYCLE;
			// write to effective address
			{
				uint16_t address = effective_address16_;
				uint8_t  value   = op(address);
				write_byte(address, value);
			}
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
