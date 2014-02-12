
#ifndef RELATIVE_H_
#define RELATIVE_H_

#define LAST_CYCLE_0                                         \
do {                                                         \
	ctx.rst_executing = false;                               \
	ctx.irq_executing = false;                               \
													         \
	if(ctx.rst_asserted) {                                   \
		ctx.rst_executing = true;                            \
	} else if(ctx.irq_asserted && ((ctx.P & I_MASK) == 0)) { \
		ctx.irq_executing = true;                            \
	}                                                        \
	ctx.rst_asserted = false;                                \
} while(0)


class relative {
public:
	relative() {
	}
	
public:
	// dispatch to the appropriate version of the address mode
	template <class Op>
	void operator()(Context &ctx, Op op) {
		execute(ctx, op, typename Op::memory_access());
	}

private:
	// this addressing mode is annoying because the CPU does
	// pipelining. This means we can't use the normal "end of opcode"
	// logic, we need to load up the next instruction and fake the
	// end of opcode sequence

	template <class Op>
	void execute(Context &ctx, Op op, const operation_branch &) {
		switch(ctx.cycle) {
		case 1:
			LAST_CYCLE;
			// fetch operand, increment ctx.PC
			data = read_byte(ctx, ctx.PC++);
			break;
		case 2:
			// Fetch opcode of next instruction,
			// If branch is taken, add operand to PCL.
			// Otherwise increment ctx.PC.
			new_op = read_byte(ctx, ctx.PC);

			// ------
			// A taken non-page-crossing branch ignores IRQ during
			// its last clock, so that next instruction executes
			// before the IRQ. Other instructions would execute the
			// NMI before the next instruction.
			// ------
			// Therefore, we DON'T check for IRQ/NMI/RESET here

			if(op(ctx)) {
				old_pc = ctx.PC;
				new_pc = (ctx.PC + static_cast<int8_t>(data));
				set_pc_lo(ctx, new_pc & 0xff);
			} else {
				if(ctx.rst_executing) {
					ctx.instruction = 0x100;
				} else if(ctx.nmi_executing) {
					ctx.instruction = 0x101;
				} else if(ctx.irq_executing) {
					ctx.instruction = 0x102;
				} else {
					ctx.instruction = new_op; ++ctx.PC;
				}
				ctx.cycle = 0;
			}
			break;
		case 3:
			// Fetch opcode of next instruction.
			// Fix PCH. If it did not change, increment ctx.PC.
			new_op = read_byte(ctx, ctx.PC);

			if((new_pc & 0xff00) != (old_pc & 0xff00)) {
				LAST_CYCLE_0;
				ctx.PC = new_pc;
			} else {
				if(ctx.rst_executing) {
					ctx.instruction = 0x100;
				} else if(ctx.nmi_executing) {
					ctx.instruction = 0x101;
				} else if(ctx.irq_executing) {
					ctx.instruction = 0x102;
				} else {
					ctx.instruction = new_op; ++ctx.PC;
				}
				ctx.cycle = 0;
			}
			break;
		case 4:
			new_op = read_byte(ctx, ctx.PC);

			if(ctx.rst_executing) {
				ctx.instruction = 0x100;
			} else if(ctx.nmi_executing) {
				ctx.instruction = 0x101;
			} else if(ctx.irq_executing) {
				ctx.instruction = 0x102;
			} else {
				ctx.instruction = new_op; ++ctx.PC;
			}
			ctx.cycle = 0;
			break;
		default:
			abort();
		}
	}

private:
	uint16_t old_pc;
	uint16_t new_pc;
	uint8_t  data;
	uint8_t  new_op;
};

#endif
