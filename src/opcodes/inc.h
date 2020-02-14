#ifndef INC_20121206_H_
#define INC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_inc
// Desc: Increment Memory
//------------------------------------------------------------------------------
struct opcode_inc {

	using memory_access = operation_modify;

	static void execute(uint8_t &data) {
		++data;
		update_nz_flags(data);
	}
};

#endif
