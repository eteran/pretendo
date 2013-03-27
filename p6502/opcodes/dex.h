#ifndef DEX_20121206_H_
#define DEX_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_dex
// Desc: Decrement X Register
//------------------------------------------------------------------------------
struct opcode_dex {

	typedef operation_none memory_access;

	void operator()() const {
		update_nz_flags(--X);
	}
};


#endif

