#ifndef JMP_20121206_H_
#define JMP_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_jmp
// Desc: Jump
//------------------------------------------------------------------------------
struct opcode_jmp {

	typedef operation_jump memory_access;

	void operator()(Context &ctx, uint16_t data) const {
		ctx.PC = data;
	}
};

#endif

