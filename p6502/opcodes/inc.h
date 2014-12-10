#ifndef INC_20121206_H_
#define INC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_inc
// Desc: Increment Memory
//------------------------------------------------------------------------------
struct opcode_inc {

	typedef operation_modify memory_access;
	
	static void execute(uint8_t &data) {
		++data;
		update_nz_flags(data);
	}
};

#endif

