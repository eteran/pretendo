#ifndef NOP_20121206_H_
#define NOP_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_nop
// Desc: No-Op
//------------------------------------------------------------------------------
struct opcode_nop {

	using memory_access = operation_read;

	// we let this take one or more parameters so
	// it can be passed to any address mode's read
	// operation
	static void execute() {
	}

	template <class T>
	static void execute(T) {
	}
};

#endif
