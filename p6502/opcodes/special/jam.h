
#ifndef JAM_20140417_H_
#define JAM_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_jam
// Desc: stall the CPU
//------------------------------------------------------------------------------
class opcode_jam {
public:
	opcode_jam() {
	}

public:
	void operator()(Context &ctx) {
		execute(ctx);
	}

private:
	void execute(Context &ctx) {
		switch(ctx.cycle) {
		case 1:
			// make sure we spin forever
			--ctx.PC;
			if(ctx.jam_handler) {
				(*ctx.jam_handler)();
			}
			OPCODE_COMPLETE;
			break;
		default:
			abort();
		}
	}
};

#endif

