#ifndef INC_20121206_H_
#define INC_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_inc
// Desc: Increment Memory
//------------------------------------------------------------------------------
struct opcode_inc {

	typedef operation_modify memory_access;
	
	void operator()(Context &ctx, uint8_t &data) const {
		++data;
		update_nz_flags(ctx, data);
	}
};

#endif

