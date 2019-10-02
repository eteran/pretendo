
#ifndef RST_20140417_H_
#define RST_20140417_H_

//------------------------------------------------------------------------------
// Name: opcode_rst
// Desc: Reset
//------------------------------------------------------------------------------
class opcode_rst {
public:
	static void execute() {
		switch(cycle_) {
		case 1:
			// read from current PC
			read_byte(PC.raw);
			break;
		case 2:
			// push PCH on stack, decrement S (fake)
			read_byte(S-- + StackAddress);
			break;
		case 3:
			// push PCL on stack, decrement S (fake)
			read_byte(S-- + StackAddress);
			break;
		case 4:
			// push P on stack, decrement S (fake)
			read_byte(S-- + StackAddress);		
			break;
		case 5:
			set_flag<I_MASK>();
			// fetch PCL
			PC.lo = read_byte(RstVectorAddress + 0);
			break;
		case 6:
			// fetch PCH
			LAST_CYCLE;
			PC.hi = read_byte(RstVectorAddress + 1);
			OPCODE_COMPLETE;
		default:
			abort();
		}
	}
};

#endif
