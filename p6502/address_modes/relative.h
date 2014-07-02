
#ifndef RELATIVE_H_
#define RELATIVE_H_

#define LAST_CYCLE_0                                  \
do {                                                  \
	rst_executing_ = false;                           \
	irq_executing_ = false;                           \
													  \
	if(rst_asserted_) {                               \
		rst_executing_ = true;                        \
	} else if(irq_asserted_ && ((P & I_MASK) == 0)) { \
		irq_executing_ = true;                        \
	}                                                 \
	rst_asserted_ = false;                            \
} while(0)


class relative {
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Op op) {
		execute(op, typename Op::memory_access());
	}

private:
	// this addressing mode is annoying because the CPU does
	// pipelining. This means we can't use the normal "end of opcode"
	// logic, we need to load up the next instruction and fake the
	// end of opcode sequence

	template <class Op>
	void execute(Op op, const operation_branch &) {
		switch(cycle_) {
		case 1:
			LAST_CYCLE;
			// fetch operand, increment PC
			data8_ = read_byte(PC++);
			break;
		case 2:
			// Fetch opcode of next instruction,
			// If branch is taken, add operand to PCL.
			// Otherwise increment PC.
			new_op = read_byte(PC);

			// ------
			// A taken non-page-crossing branch ignores IRQ during
			// its last clock, so that next instruction executes
			// before the IRQ. Other instructions would execute the
			// NMI before the next instruction.
			// ------
			// Therefore, we DON'T check for IRQ/NMI/RESET here

			if(op()) {
				old_pc = PC;
				new_pc = (PC + static_cast<int8_t>(data8_));
				set_pc_lo(new_pc & 0xff);
			} else {
				if(rst_executing_) {
					instruction_ = 0x100;
				} else if(nmi_executing_) {
					instruction_ = 0x101;
				} else if(irq_executing_) {
					instruction_ = 0x102;
				} else {
					instruction_ = new_op; ++PC;
				}
				cycle_ = 0;
			}
			break;
		case 3:
			// Fetch opcode of next instruction.
			// Fix PCH. If it did not change, increment PC.
			new_op = read_byte(PC);

			if((new_pc & 0xff00) != (old_pc & 0xff00)) {
				LAST_CYCLE_0;
				PC = new_pc;
			} else {
				if(rst_executing_) {
					instruction_ = 0x100;
				} else if(nmi_executing_) {
					instruction_ = 0x101;
				} else if(irq_executing_) {
					instruction_ = 0x102;
				} else {
					instruction_ = new_op; ++PC;
				}
				cycle_ = 0;
			}
			break;
		case 4:
			new_op = read_byte(PC);

			if(rst_executing_) {
				instruction_ = 0x100;
			} else if(nmi_executing_) {
				instruction_ = 0x101;
			} else if(irq_executing_) {
				instruction_ = 0x102;
			} else {
				instruction_ = new_op; ++PC;
			}
			cycle_ = 0;
			break;
		default:
			abort();
		}
	}

private:
	uint16_t old_pc;
	uint16_t new_pc;
	uint8_t  new_op;
};

#endif
