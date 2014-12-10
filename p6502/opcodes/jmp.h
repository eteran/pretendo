#ifndef JMP_20121206_H_
#define JMP_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_jmp
// Desc: Jump
//------------------------------------------------------------------------------
struct opcode_jmp {

	typedef operation_jump memory_access;

	static void execute(uint16_t data) {
		PC.raw = data;
	}
};

#endif

