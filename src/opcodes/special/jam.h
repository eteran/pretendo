
#ifndef JAM_20140417_H_
#define JAM_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_jam
// Desc: stall the CPU
//------------------------------------------------------------------------------
class opcode_jam {
public:
	static void execute() {
		switch(cycle_) {
		case 1:
			// make sure we spin forever
			--PC.raw;
			jam_handler();
			OPCODE_COMPLETE;
			break;
		default:
			abort();
		}
	}
};

#endif

