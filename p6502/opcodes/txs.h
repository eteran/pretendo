#ifndef TXS_20121206_H_
#define TXS_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_txs
// Desc: Transfer X to S
//------------------------------------------------------------------------------
struct opcode_txs {

	typedef operation_none memory_access;

	static void execute() {
		S = X;
	}
};


#endif

