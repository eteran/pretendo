
#ifndef RELATIVE_H_
#define RELATIVE_H_

#define LAST_CYCLE_0                                       \
	do {                                                   \
		rst_executing_ = false;                            \
		irq_executing_ = false;                            \
                                                           \
		if (rst_asserted_) {                               \
			rst_executing_ = true;                         \
		} else if (irq_asserted_ && ((P & I_MASK) == 0)) { \
			irq_executing_ = true;                         \
		}                                                  \
		rst_asserted_ = false;                             \
	} while (0)

template <class Op>
class relative {
public:
	// dispatch to the appropriate version of the address mode
	static void execute() {
		execute(typename Op::memory_access());
	}

private:
	// this addressing mode is annoying because the CPU does
	// pipelining. This means we can't use the normal "end of opcode"
	// logic, we need to load up the next instruction and fake the
	// end of opcode sequence
	static void execute(const operation_branch &) {
		switch (cycle_) {
		case 1:
			LAST_CYCLE;
			// fetch operand, increment PC
			data8_ = read_byte(PC.raw++);
			break;
		case 2: {
			// Fetch opcode of next instruction,
			// If branch is taken, add operand to PCL.
			// Otherwise increment PC.

			// ------
			// A taken non-page-crossing branch ignores IRQ during
			// its last clock, so that next instruction executes
			// before the IRQ. Other instructions would execute the
			// NMI before the next instruction.
			// ------
			// Therefore, we DON'T check for IRQ/NMI/RESET here

			const uint8_t next_op = read_byte(PC.raw);

			if (Op::execute()) {
				old_pc_.raw = PC.raw;
				new_pc_.raw = (PC.raw + static_cast<int8_t>(data8_));
				PC.lo       = new_pc_.lo;
			} else {
				cycle_0(next_op);
				cycle_ = 0;
			}
			break;
		}
		case 3: {
			// Fetch opcode of next instruction.
			// Fix PCH. If it did not change, increment PC.

			const uint8_t next_op = read_byte(PC.raw);

			if (new_pc_.hi != old_pc_.hi) {
				LAST_CYCLE_0;
				PC.raw = new_pc_.raw;
				OPCODE_COMPLETE;
			} else {
				cycle_0(next_op);
				cycle_ = 0;
			}
			break;
		}
		default:
			abort();
		}
	}
};

#undef LAST_CYCLE_0

#endif
