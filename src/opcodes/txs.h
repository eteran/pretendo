#ifndef TXS_20121206_H_
#define TXS_20121206_H_

//------------------------------------------------------------------------------
// Name: opcode_txs
// Desc: Transfer X to S
//------------------------------------------------------------------------------
struct opcode_txs {

	using memory_access = operation_none;

	static void execute() {
		S = X;
	}
};

#endif
