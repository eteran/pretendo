#ifndef JMP_20121206_H_
#define JMP_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_jmp
// Desc: Jump
//------------------------------------------------------------------------------
struct opcode_jmp {

    using memory_access = operation_jump;

	static void execute(uint16_t data) {
		PC.raw = data;
	}
};

#endif

