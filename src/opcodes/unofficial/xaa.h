#ifndef XAA_20121206_H_
#define XAA_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_xaa
// Desc: Logical Inclusive OR
//------------------------------------------------------------------------------
struct opcode_xaa {

	using memory_access = operation_read;

	static void execute(uint8_t data) {

		// who knows what the magic value should be...

		static const uint8_t magic = 0x00;

		A = (A | magic) & X & data;
		update_nz_flags(A);
	}
};

#endif
